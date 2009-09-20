#include "server.h"

#include "json/json.h"

#include <fstream>
#include <boost/filesystem.hpp>

#include "game/map_loader/map_loader.h"

#include "md5/md5.h"
#include "base64/base64.h"

namespace json = Json;

//Some convenience-methods
namespace {
  std::string get_color_string(aw::player::colors c) {
	switch(c) {
	case aw::player::RED:
	  return "red";
	case aw::player::BLUE:
	  return "blue";
	case aw::player::GREEN:
	  return "green";
	case aw::player::YELLOW:
	  return "yellow";
	case aw::player::BLACK:
	  return "black";
	default:
	  return "FAIL";
	}
  }

  aw::player::colors get_color_from_string(const std::string& c) {
	if(c == "red")
	  return aw::player::RED;
	else if(c == "blue")
	  return aw::player::BLUE;
	else if(c == "greeen")
	  return aw::player::GREEN;
	else if(c == "yellow")
	  return aw::player::YELLOW;
	else if(c == "black")
	  return aw::player::BLACK;
	else
	  throw std::runtime_error("wrong color-string");
  }

  //Needs to be configurable
  const std::string map_dir = "maps/";
}

server::server(asio::io_service& io)
  : io_service_(io), acceptor_(io, tcp::endpoint(tcp::v4(), 4242)) {

}

void server::start_accept() {
  client_connection::ptr new_connection =
	client_connection::create(io_service_);

  acceptor_.async_accept(new_connection->socket(),
						 boost::bind(&server::handle_accept, 
									 this, 
									 new_connection,
									 boost::asio::placeholders::error));
}

void server::deliver_to_all(const std::string& message) {
  std::clog << "Delivering message to all clients" << std::endl;
  BOOST_FOREACH(client_connection::ptr& c, connections_) {
	c->send_message(message);
  }
}

void server::deliver_to_all_except(const client_connection::ptr& except, 
								   const std::string& message) {
  BOOST_FOREACH(client_connection::ptr& c, connections_) {
	if(c != except)
	  deliver_to(c, message);
  }
}

void server::deliver_to(client_connection::ptr& to, const std::string& message) {
  to->send_message(message);
}

void server::handle_message(const std::string& message, 
							client_connection::ptr from) {
  try {
	json::Value root;
	json::Reader reader;
	if(!reader.parse(message, root))
	  throw std::runtime_error(reader.getFormatedErrorMessages());

	json::Value destination = root.get("destination", json::Value());

	if(!destination) 
	  throw std::runtime_error("Didn't find the key 'destination'");

	std::string dest = destination.asString();

	if(dest == "server") {
	  handle_server_message(root, from);
	} else if(dest == "client") {
	  //Just pass it to all other clients
	  deliver_to_all_except(from, message);
	} else {
	  std::clog << "Destionation unknown (" << dest 
				<< "), discarding it" << std::endl;
	}
  } catch(const std::exception& e) {
	std::clog << "Got invalid json: " << message << std::endl;
  }
}

void server::handle_lost_connection(const std::string& reason, 
							const client_connection::ptr& from) {
  std::clog << "Client lost connection. Reason: " << reason << std::endl;
  
  //Remove that client from the list
  connections_.remove(from);

  //Inform all clients that a client lost its connection
  json::Value root;
  root["type"] = "connection-lost";
  root["destination"] = "client";
  root["reason"] = reason;
  root["player"] = serialize_client_connection(from);
  
  deliver_to_all(write_json(root));
}

void server::handle_accept(client_connection::ptr new_connection,
						   const boost::system::error_code& error) {
  if (!error) {
	std::clog << "Got a new client" << std::endl;

	//If the server is empty, make this user the host
	if(connections_.empty()) {
	  new_connection->is_host = true;
	  std::clog << "New connection is now host!" << std::endl;
	}

	connections_.push_back(new_connection);
	new_connection->deliver_callback().connect(boost::bind(&server::handle_message,this,_1,_2));
	new_connection->connection_lost_callback().connect(boost::bind(&server::handle_lost_connection,this,_1,_2));
	new_connection->start();

	//Inform the client that he is the host now
	json::Value root;
	root["destination"] = "client";
	root["type"] = "server-status";
	root["host"] = new_connection->is_host;
	root["map"] = json::Value();

	json::Value players(json::arrayValue);
	BOOST_FOREACH(client_connection::ptr& c, connections_) {
	  if(c != new_connection)
		players.append(serialize_client_connection(c));
	}

	root["players"] = players;

	json::Value available_colors(json::arrayValue);
	BOOST_FOREACH(const std::string& c, get_available_colors())
	  available_colors.append(c);
	root["available-colors"] = available_colors;

	deliver_to(new_connection, write_json(root));
	
	start_accept();
  }
}

std::string server::write_json(const Json::Value& v) {
  json::FastWriter writer;
  return writer.write(v);
}

void server::handle_server_message(const json::Value& root,
								   client_connection::ptr from) {
  const std::string type = root.get("type", "UNDEFINED").asString();
  
  if(type == "UNDEFINED") {
	throw std::runtime_error("Type for a server-message is undefined");

  } else if(type == "set-username") {
	std::string old_username = from->username;
	std::string new_username = root.get("new-username", 
										old_username).asString();


	  
	if(old_username != new_username) {
	  from->username = new_username;

	  //Notify clients
	  json::Value notify;
	  notify["destination"] = "client";
	  notify["type"] = "username-changed";
	  notify["old-name"] = old_username;
	  notify["new-name"] = new_username;

	  deliver_to_all_except(from, write_json(notify));
	}
  } else if(type == "choose-color") {
	try {
	  std::string color = root.get("chosen-color", "UNDEFINED").asString();

	  bool available = false;

	  //Check if the color is available
	  std::list<std::string> available_colors(get_available_colors());
	  if(std::find(available_colors.begin(), available_colors.end(), color) !=
		 available_colors.end())
		available = true;

	  if(color == "UNDEFINED" || !available) {
		//small hack to avoid code-duplication if get_color_from_string throws
		throw std::runtime_error("Chosen color not available");
	  } else {
		//Everything is fine. Set the new color and inform other clients
	  
		std::string old_color = get_color_string(from->color);
		from->color = get_color_from_string(color);
		
		json::Value root;
		root["destination"] = "client";
		root["type"] = "color-changed";
		root["new-color"] = color;
		root["player"] = serialize_client_connection(from);

		deliver_to_all_except(from, write_json(root));
	 
	  }
	} catch(const std::runtime_error& e) {
	  //color not available or json is damaged. Create an error-response
	  json::Value error = create_error_response(type, 
												e.what());
		
	  json::Value available_colors(json::arrayValue);
	  BOOST_FOREACH(const std::string& c, get_available_colors())
		available_colors.append(c);
	  error["available-colors"] = available_colors;
		
	  deliver_to(from, write_json(error));
	}
  } else if(type == "load-map") {
	try {
	  if(!from->is_host)
		throw std::runtime_error("Only hosts can load maps");
	  
	  map_filename_ = root.get("map-file", 
							   "UNDEFINED").asString();

	  
	  //TODO: Check if the md5sum matches

	  //When the file can't be found, ask the host for the data
	  if(!boost::filesystem::exists(map_dir + map_filename_)) {
		std::clog << map_dir + map_filename_
				  << " doesn't exists. Asking host for data" << std::endl;

		json::Value root;
		root["destination"] = "client";
		root["type"] = "map-data";
		root["filename"] = map_filename_;
		
		deliver_to(from, write_json(root));

		//Return, as we have to wait for the data
		return;
	  }

	  //Load map
	  aw::map_loader loader;
	  aw::map_loader::loaded_map::ptr loaded_map = loader.load(map_dir + map_filename_);
	  map_ = aw::map::ptr(new aw::map(loaded_map->m_terrain, 
									  loaded_map->m_unit));

	  //Load map-data
	  std::ifstream is(std::string(map_dir + map_filename_).c_str());
	  is.seekg(0, std::ios::end);
	  map_data_size_ = is.tellg();
	  is.seekg(0, std::ios::beg);

	  map_data_ = boost::shared_array<unsigned char>(new unsigned char[map_data_size_]);
	  is.read(reinterpret_cast<char*>(map_data_.get()), map_data_size_);
	  is.close();

	  //Deliver this to all other clients
	  deliver_to_all_except(from, 
							write_json(create_map_data_response()));

	} catch(const std::runtime_error& e) {
	  //Deliver an errror-message to the sender
	  deliver_to(from, write_json(create_error_response(type, e.what())));
	}
  } else if(type == "get-map") {
	if(map_ != NULL) {
	  deliver_to(from, write_json(create_map_data_response()));
	} else {
	  json::Value error = create_error_response(type, 
												"No map was loaded by the host");
	  deliver_to(from, write_json(error));
	}
  }
}

json::Value server::serialize_client_connection(const client_connection::ptr& ptr) {
  json::Value root;
  root["name"] = ptr->username;
  root["is-host"] = ptr->is_host;
  root["is-spectator"] = ptr->is_spectator;

  if(ptr->is_spectator) {
	root["color"] = json::Value();
  } else {
	root["color"] = get_color_string(ptr->color);
  }

  return root;
}

std::list<std::string> server::get_available_colors() {
  static aw::player::colors colors[] = { aw::player::RED, 
										 aw::player::BLUE,
										 aw::player::GREEN,
										 aw::player::YELLOW,
										 aw::player::BLACK };
  
  std::list<aw::player::colors> coltemp(colors, colors+5);

  BOOST_FOREACH(client_connection::ptr& c, connections_) {
	if(!c->is_spectator)
	  coltemp.remove(c->color);
  }

  std::list<std::string> ret;
  BOOST_FOREACH(aw::player::colors c, coltemp)
	ret.push_back(get_color_string(c));

  return ret;
}

 json::Value server::create_error_response(const std::string& request,
										   const std::string& reason) {
   json::Value root;
   root["destination"] = "client";
   root["type"] = "error";
   root["request"] = request;
   root["reason"] = reason;

   return root;
 }

Json::Value server::create_map_data_response() {
  json::Value root;
  root["destination"] = "server";
  root["type"] = "map-loaded";
  root["filename"] = map_filename_;
  
  MD5 md5;
  md5.update(reinterpret_cast<char*>(map_data_.get()), map_data_size_);
  md5.finalize();
  root["md5sum"] = md5.hexdigest();
  root["data"] = base64_encode(reinterpret_cast<unsigned char*>(map_data_.get()),
							   map_data_size_);

  return root;
}

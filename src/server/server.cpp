#include "server.h"

#include "json/json.h"

namespace json = Json;

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
							const client_connection::ptr& from) {
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
  root["player"] = "NONAME";
  
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

	deliver_to(new_connection, write_json(root));
	
	start_accept();
  }
}

std::string server::write_json(const Json::Value& v) {
  json::FastWriter writer;
  return writer.write(v);
}

void server::handle_server_message(const json::Value& root,
								   const client_connection::ptr& from) {
  const std::string type = root.get("type", "UNDEFINED").asString();
  
  if(type == "UNDEFINED")
	throw std::runtime_error("Type for a server-message is undefined");
  else if(type == "set-username") {
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
  }
}

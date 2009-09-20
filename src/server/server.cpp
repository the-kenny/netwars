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
	  //Handle it for the server
	} else if(dest == "client") {
	  //Just pass it to all other clients

	  BOOST_FOREACH(client_connection::ptr& c, connections_) {
		if(c != from)
		  deliver_to(c, message);
	  }
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
  
  json::FastWriter writer;
  deliver_to_all(writer.write(root));
}

void server::handle_accept(client_connection::ptr new_connection,
						   const boost::system::error_code& error) {
  if (!error) {
	std::clog << "Got a new client" << std::endl;
	connections_.push_back(new_connection);
	new_connection->deliver_callback().connect(boost::bind(&server::handle_message,this,_1,_2));
	new_connection->connection_lost_callback().connect(boost::bind(&server::handle_lost_connection,this,_1,_2));
	new_connection->start();
	start_accept();
  }
}

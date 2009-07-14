#include "server.h"

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
  BOOST_FOREACH(const client_connection::ptr& c, conections_) {
	c->send_message(message);
  }
}

void server::deliver_to(client_connection::ptr& to, const std::string& message) {
  to->send_message(message);
}

void server::handle_accept(client_connection::ptr new_connection,
				   const boost::system::error_code& error) {
  if (!error) {
	std::clog << "Got a new client" << std::endl;
	conections_.push_back(new_connection);
	new_connection->deliver_callback().connect(boost::bind(&server::deliver_to_all,this,_1));
	new_connection->start();
	start_accept();
  }
}

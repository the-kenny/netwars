#ifndef AW_NETWORK_SERVER_H
#define AW_NETWORK_SERVER_H

#include <boost/asio.hpp>
#include <boost/foreach.hpp>

#include "client_connection.h"

using namespace boost;
using boost::asio::ip::tcp;

class server {
public:
  server(asio::io_service& io)
	: io_service_(io), acceptor_(io, tcp::endpoint(tcp::v4(), 4242)) {

  }

   void start_accept() {
	 client_connection::ptr new_connection =
	   client_connection::create(io_service_);

    acceptor_.async_accept(new_connection->socket(),
						   boost::bind(&server::handle_accept, 
									   this, 
									   new_connection,
									   boost::asio::placeholders::error));
  }

  void deliver(const std::string& message) {
	BOOST_FOREACH(const client_connection::ptr& c, conections_) {
	  c->send_message(message);
	}
  }

private:
 void handle_accept(client_connection::ptr new_connection,
					const boost::system::error_code& error) {
    if (!error) {
	  conections_.push_back(new_connection);
	  new_connection->deliver_callback().connect(boost::bind(&server::deliver,this,_1));
      new_connection->start();
      start_accept();
    }
  }

private:
  asio::io_service& io_service_;
  asio::ip::tcp::acceptor acceptor_;


  std::list<client_connection::ptr> conections_;
};

#endif

#include <boost/asio.hpp>

#include "game/networking/connection.h"

using namespace boost;
using boost::asio::ip::tcp;

class server {
public:
  server(asio::io_service& io)
	: io_service_(io), acceptor_(io, tcp::endpoint(tcp::v4(), 4242)) {

  }

   void start_accept() {
    tcp_connection::ptr new_connection =
      tcp_connection::create(io_service_);

    acceptor_.async_accept(new_connection->socket(),
						   boost::bind(&server::handle_accept, 
									   this, 
									   new_connection,
									   boost::asio::placeholders::error));
  }

private:
 void handle_accept(tcp_connection::ptr new_connection,
					const boost::system::error_code& error) {
    if (!error) {
	  conections_.push_back(new_connection);
      new_connection->start();
      start_accept();
    }
  }

private:
  asio::io_service& io_service_;
  asio::ip::tcp::acceptor acceptor_;


  std::list<tcp_connection::ptr> conections_;

};

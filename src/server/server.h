#ifndef AW_NETWORK_SERVER_H
#define AW_NETWORK_SERVER_H

#include <boost/asio.hpp>
#include <boost/foreach.hpp>

#include "client_connection.h"

using namespace boost;
using boost::asio::ip::tcp;

class server {
public:
  server(asio::io_service& io);

  void start_accept();

  void deliver_to_all(const std::string& message);
  void deliver_to(client_connection::ptr& to, const std::string& message);
  void handle_message(const std::string& message, 
					  const client_connection::ptr& from);


private:
 void handle_accept(client_connection::ptr new_connection,
					const boost::system::error_code& error);

private:
  asio::io_service& io_service_;
  asio::ip::tcp::acceptor acceptor_;


  std::list<client_connection::ptr> conections_;
};

#endif

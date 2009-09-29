#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <iostream>

#include "game/networking/connection.h"

int main() {
  boost::asio::io_service io_service;

  tcp_connection::ptr conn(tcp_connection::create(io_service));
  conn->connect("localhost", "4242");

  boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

  std::string message;
  while(message != "quit") {
	std::getline(std::cin, message);
	conn->send_message(message);

	if(conn->has_line()) {
	  while(conn->has_line())
		std::cout << conn->receive_line() << std::endl;
	}
  }
	
  t.join();
}

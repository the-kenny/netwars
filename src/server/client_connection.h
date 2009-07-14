#ifndef AW_NETWORKING_CLIENT_CONNECTION_H
#define AW_NETWORKING_CLIENT_CONNECTION_H

#include "game/networking/connection.h"

#include <boost/signals.hpp>

class client_connection: public connection {
public:
  typedef boost::shared_ptr<client_connection> ptr;

  static ptr create(asio::io_service& io) {
	return ptr(new client_connection(io));
  }

  boost::signal<void(const std::string&)> &deliver_callback() {
	return deliver_callback_;
  }

protected:
  void on_line_received(const std::string& line) {
	//We simply deliver all messages unchanged to the other clients
   
	if(!deliver_callback_.empty()) {
	  deliver_callback_(line);
	  receive_line();
	} else {
	  throw std::logic_error("No deliver-callback defined.");
	}
  }

private:
  client_connection(asio::io_service& io)
	: connection(io) {
  }

  boost::signal<void(const std::string&)> deliver_callback_;
};

#endif

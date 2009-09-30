#ifndef AW_NETWORK_SERVER_CONNECTION_H
#define AW_NETWORK_SERVER_CONNECTION_H

#include "connection.h"

#include <boost/thread.hpp>

namespace aw {
  class server_connection: public connection {
  public:
	typedef boost::shared_ptr<server_connection> ptr;
	
	

	virtual ~server_connection() {
	}

	static ptr create(boost::asio::io_service& io_service) {
	  return ptr(new server_connection(io_service));
	}

	bool connection_lost() {
	  boost::unique_lock<boost::mutex> lock(read_lock_);
	  return connection_lost_;
	}

	virtual bool has_line() {
	  boost::unique_lock<boost::mutex> lock(read_lock_);
	  return connection::has_line();
	}
	
	std::string receive_line() {
	  boost::unique_lock<boost::mutex> lock(read_lock_);
	  return connection::receive_line();
	}
	
	void send_message(const std::string& message) {
	  io_service_.post(boost::bind(&connection::send_message, this, message));
	}
	
  protected:
	server_connection(boost::asio::io_service& io_service) 
	  : connection(io_service) {
	}

	void on_line_received(const std::string& line) {
	  boost::unique_lock<boost::mutex> lock(read_lock_);
	  connection::on_line_received(line);
	}

	void on_connection_closed(const std::string& reason) {
	  boost::unique_lock<boost::mutex> lock(read_lock_);
	  connection::on_connection_closed(reason);
	  connection_lost_ = true;
	}
	
  private:
	boost::mutex read_lock_;
	bool connection_lost_;
  };
}

#endif

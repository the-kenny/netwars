#include "connection.h"

#include <boost/lexical_cast.hpp>

using namespace aw;

connection::ptr connection::create(boost::asio::io_service& io_service) {
  return ptr(new connection(io_service));
}

tcp::socket& connection::socket() {
  return socket_;
}

void connection::send_message(const std::string& message) {
  io_service_.post(boost::bind(&connection::do_write, 
							   this, 
							   message));
}

void connection::connect(const std::string& host, const std::string& port) {
  tcp::resolver resolver(io_service_);
  tcp::resolver::query query(host, port);
	
  tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	
  socket_.async_connect(*endpoint_iterator,
						boost::bind(&connection::handle_connect, 
									this,
									boost::asio::placeholders::error,
									++endpoint_iterator));
}

void connection::start() {
  boost::asio::async_read_until(socket_, buffer_, "\n", 
								boost::bind(&connection::handle_read,
											this,
											boost::asio::placeholders::error));
}


bool connection::has_line() const {
  return !receive_queue_.empty();
}

std::string connection::receive_line() {
  std::string line(receive_queue_.front());
  receive_queue_.pop_front();
  return line;
}

void connection::on_line_received(const std::string&) {
  receive_queue_.pop_back();
}

void connection::on_connection_closed(const std::string& reason) {
  //socket_.close(); //This crashes somehow the server, we just leave it open
}

connection::connection(boost::asio::io_service& io_service)
  : socket_(io_service), io_service_(io_service) {

}

void connection::handle_connect(const boost::system::error_code& error,
									tcp::resolver::iterator endpoint_iterator)
{
  if (!error) {
	this->start();
  } else if (endpoint_iterator != tcp::resolver::iterator()) {
	socket_.close();
	tcp::endpoint endpoint = *endpoint_iterator;
	socket_.async_connect(endpoint,
						  boost::bind(&connection::handle_connect, 
									  this,
									  boost::asio::placeholders::error, 
									  ++endpoint_iterator));
  } else {
	throw std::runtime_error("Couldn't resolve hostname");
  }
}


void connection::do_write(std::string message) {
  //std::cout << "do_write called" << std::endl;

  bool write_in_progress = !send_queue_.empty();
  send_queue_.push_back(message);
  if (!write_in_progress) {
	boost::asio::async_write(socket_, 
							 boost::asio::buffer(send_queue_.front()),
							 boost::bind(&connection::handle_write, 
										 this,
										 boost::asio::placeholders::error));
  }
}

void connection::handle_write(const boost::system::error_code& error) {
  if(!error) {
	send_queue_.pop_front();

	if (!send_queue_.empty()) {
	  boost::asio::async_write(socket_,
							   boost::asio::buffer(send_queue_.front()),
							   boost::bind(&connection::handle_write, 
										   this,
										   boost::asio::placeholders::error));
	}
  } else {
	on_connection_closed(error.message());
  }
}

void connection::handle_read(const boost::system::error_code& error) {
  if(!error) {
	std::istream is(&buffer_);
	std::string line;
	
	std::getline(is, line);
	receive_queue_.push_back(line);
	on_line_received(line);
	
	
	boost::asio::async_read_until(socket_, buffer_, "\n", 
								  boost::bind(&connection::handle_read,
											  this,
											  boost::asio::placeholders::error));
  }  else {
	on_connection_closed(error.message());
  }
}

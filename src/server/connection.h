#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include <iostream>
#include <string>
#include <deque>

using boost::asio::ip::tcp;

class tcp_connection
  : public boost::enable_shared_from_this<tcp_connection> {
public:
  typedef boost::shared_ptr<tcp_connection> pointer;

  static pointer create(boost::asio::io_service& io_service) {
    return pointer(new tcp_connection(io_service));
  }

  tcp::socket& socket() {
    return socket_;
  }

  void send_message(const std::string& message) {
    io_service_.post(boost::bind(&tcp_connection::do_write, 
								 this, 
								 message));
  }

  void start() {
	boost::asio::async_read_until(socket_, buffer_, "\n", 
								  boost::bind(&tcp_connection::handle_read,
											  this,
											  boost::asio::placeholders::error));
  }


  bool has_line() const {
	return !receive_queue_.empty();
  }

  std::string receive_line() {
	std::string line(receive_queue_.front());
	receive_queue_.pop_front();
	return line;
  }

private:
  tcp_connection(boost::asio::io_service& io_service)
    : socket_(io_service), io_service_(io_service) {

  }

  void do_write(std::string message) {
	std::cout << "do_write called" << std::endl;

	bool write_in_progress = !send_queue_.empty();
    send_queue_.push_back(message);
    if (!write_in_progress) {
	  boost::asio::async_write(socket_, 
							   boost::asio::buffer(send_queue_.front()),
							   boost::bind(&tcp_connection::handle_write, 
										   this,
										   boost::asio::placeholders::error));
	}
  }

  void handle_write(const boost::system::error_code& error) {
	if(!error) {
	  send_queue_.pop_front();

	if (!send_queue_.empty()) {
	  boost::asio::async_write(socket_,
							   boost::asio::buffer(send_queue_.front()),
							   boost::bind(&tcp_connection::handle_write, 
										   this,
										   boost::asio::placeholders::error));
      }
	}
  }

  void handle_read(const boost::system::error_code&) {
	std::istream is(&buffer_);
	std::string line;
	
	std::getline(is, line);
	receive_queue_.push_back(line);
	  
	
	boost::asio::async_read_until(socket_, buffer_, "\n", 
								  boost::bind(&tcp_connection::handle_read,
											  this,
											  boost::asio::placeholders::error));
  }


  tcp::socket socket_;
  boost::asio::io_service& io_service_;
  boost::asio::streambuf buffer_;

  std::deque<std::string> send_queue_;
  std::deque<std::string> receive_queue_;
};

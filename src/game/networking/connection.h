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
  typedef boost::shared_ptr<tcp_connection> ptr;

  static ptr create(boost::asio::io_service& io_service);

  void send_message(const std::string& message);
  void connect(const std::string& host, const std::string& port);
  void start();

  tcp::socket& socket();
  bool has_line() const;
  std::string receive_line();

private:
  tcp_connection(boost::asio::io_service& io_service);

  void do_write(std::string message);

  void handle_connect(const boost::system::error_code& error,
					  tcp::resolver::iterator endpoint_iterator);
  void handle_write(const boost::system::error_code& error);
  void handle_read(const boost::system::error_code&);


  tcp::socket socket_;
  boost::asio::io_service& io_service_;
  boost::asio::streambuf buffer_;

  std::deque<std::string> send_queue_;
  std::deque<std::string> receive_queue_;
};

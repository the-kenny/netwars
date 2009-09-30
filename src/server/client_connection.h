#ifndef AW_NETWORKING_CLIENT_CONNECTION_H
#define AW_NETWORKING_CLIENT_CONNECTION_H

#include "game/networking/connection.h"

#include <boost/signals2/signal.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "game/player.h"

using namespace boost;

class client_connection: public aw::connection, public boost::enable_shared_from_this<client_connection> {

public:
  typedef boost::shared_ptr<client_connection> ptr;
  typedef signals2::signal<void
						   (const std::string&,
							const client_connection::ptr&)> general_signal_t;

  static ptr create(asio::io_service& io) {
	return ptr(new client_connection(io));
  }

  general_signal_t &deliver_callback() {
	return deliver_callback_;
  }

  general_signal_t &connection_lost_callback() {
	return connection_lost_callback_;
  }


  //Game-Specific things
  //I know this is ugly, but I'm to lazy for all these accessor-methods
  bool is_host;
  bool is_spectator;
  std::string username;
  aw::player::colors color;

protected:
  void on_line_received(const std::string& line);
  void on_connection_closed(const std::string& reason);

private:
  client_connection(asio::io_service& io)
	: connection(io),
	  is_host(false),
	  is_spectator(true),
	  username("") {
  }

  general_signal_t deliver_callback_;
  general_signal_t connection_lost_callback_;
};

#endif

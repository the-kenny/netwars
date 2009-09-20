#include "server.h"

#include "game/config.h"

int main() {
  boost::asio::io_service io;
  
  aw::config::instance().load("config.xml");

  server s(io);
  s.start_accept();

  io.run();
}

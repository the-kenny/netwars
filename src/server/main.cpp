#include "server.h"


int main() {
  boost::asio::io_service io;

  server s(io);
  s.start_accept();

  io.run();
}

#include "client_connection.h"


void client_connection::on_line_received(const std::string& line) {
  //We simply deliver all messages unchanged to the other clients
   
  if(!deliver_callback_.empty()) {
	deliver_callback_(line, shared_from_this());
	receive_line();
  } else {
	throw std::logic_error("No deliver-callback defined.");
  }
}

void client_connection::on_connection_closed(const std::string& reason) {
  if(!connection_lost_callback_.empty()) {
	connection_lost_callback_(reason, shared_from_this());

	//Call super (to close socket)
	aw::connection::on_connection_closed(reason);
  } else {
	throw std::logic_error("No connection-lost-callback defined.");
  }
}

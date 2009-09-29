#ifndef AW_NETWORK_SERVER_H
#define AW_NETWORK_SERVER_H

#include <boost/asio.hpp>
#include <boost/foreach.hpp>

#include "client_connection.h"

#include "json/json.h"
#include "game/player.h"
#include "game/map.h"

using namespace boost;
using boost::asio::ip::tcp;

class server {
public:
  server(asio::io_service& io);

  void start_accept();

  void deliver_to_all(const std::string& message);

  void deliver_to_all_except(const client_connection::ptr& except, 
							 const std::string& message);
  void deliver_to(client_connection::ptr& to, const std::string& message);
  void handle_message(const std::string& message, 
					  client_connection::ptr from);


private:
  void handle_accept(client_connection::ptr new_connection,
					 const boost::system::error_code& error);
  void handle_lost_connection(const std::string& reason, 
							  const client_connection::ptr& from);

  void handle_server_message(const Json::Value& root,
							 client_connection::ptr from);

  //Convenience-method
  std::string write_json(const Json::Value& v);
  Json::Value serialize_client_connection(const client_connection::ptr& ptr);
  Json::Value serialize_map_metadata(const aw::map::ptr& map);
  
  std::list<std::string> get_available_colors();

  Json::Value create_error_response(const std::string& request,
									const std::string& reason);
  Json::Value create_map_data_response();
						  
private:
  asio::io_service& io_service_;
  asio::ip::tcp::acceptor acceptor_;

  std::list<client_connection::ptr> connections_;

  aw::map::ptr map_;
  std::string map_filename_;
  boost::shared_array<unsigned char> map_data_;
  std::size_t map_data_size_;

  std::string map_dir;
};

#endif

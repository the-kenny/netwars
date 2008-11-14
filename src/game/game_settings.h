#ifndef AW_GAME_SETTINGS_H
	#define AW_GAME_SETTINGS_H

#include "player.h"
#include "map.h"

#include <string>

namespace aw
{
	class game_settings
	{
		public:
			std::string get_map_filename() const { return m_map_filename; }
			map::ptr get_loaded_map() const { return m_map; }

			int get_funds_per_building() const { return m_funds_per_building; }
			int get_initial_funds() const { return m_initial_funds; }
			player_manager get_players() const { return m_players; }

			void set_funds_per_building(int fpb) { m_funds_per_building = fpb; }
			void set_initial_funds(int funds) { m_initial_funds = funds; }
			void set_players(const player_manager &p);

			void load_map(const std::string &filename);

		private:
			std::string m_map_filename;
			map::ptr m_map;

			int m_funds_per_building;
			int m_initial_funds;

			player_manager m_players;
	};
}

#endif

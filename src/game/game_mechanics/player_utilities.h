#ifndef AW_PLAYER_UTILITIES_H
	#define AW_PLAYER_UTILITIES_H

#include "game/map.h"
#include "game/player.h"

#include <string>

namespace aw
{
	namespace game_mechanics
	{
		enum defeat_type { NOT_BEATEN, ALL_UNITS_BEATEN, HQ_CAPTURED };
		defeat_type beaten(const map::ptr &map, const player::ptr &player);

		bool participates(const map::ptr &map, player::colors c);

	  player::colors color_from_string(const std::string& c);
	  std::string color_to_string(player::colors c);
	}
}

#endif

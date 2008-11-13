#ifndef AW_PLAYER_UTILITIES_H
	#define AW_PLAYER_UTILITIES_H

#include "../map.h"
#include "../player.h"

namespace aw
{
	namespace game_mechanics
	{
		enum defeat_type { NOT_BEATEN, ALL_UNITS_BEATEN, HQ_CAPTURED };
		defeat_type beaten(const map::ptr &map, const player::ptr &player);

		bool participates(const map::ptr &map, player::colors c);
	}
}

#endif

#ifndef AW_SUPPLY_MECHANICS_H
	#define AW_SUPPLY_MECHANICS_H


#include "game/map.h"
#include "game/coord.h"
#include "game/area.h"

namespace aw
{
	namespace game_mechanics
	{
		bool can_repair(const map::ptr &map, const coord &unit_c, int funds);
		bool can_supply(const map::ptr &map, const coord &unit_c);

		bool can_repair(const map::ptr &map, const coord &unit_c, const coord &target_c, int funds);
		bool can_supply(const map::ptr &map, const coord &unit_c, const coord &target_c);

		aw::area get_supply_coordinates(const map::ptr &map, const coord &unit_c);
		aw::area get_repair_coordinates(const map::ptr &map, const coord &unit_c, int funds);
	}
}

#endif

#ifndef AW_MOVE_UTILITIES_H
	#define AW_MOVE_UTILITIES_H

#include "game/map.h"
#include "game/units/unit_base.h"
#include "game/coord.h"

namespace aw
{
	namespace game_mechanics
	{
		bool can_pass(const map::ptr &map, const coord &c, unit::ptr unit);
	}
}

#endif

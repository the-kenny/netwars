#ifndef AW_MOVE_UTILITIES_H
	#define AW_MOVE_UTILITIES_H

#include "map.h"
#include "units/unit_base.h"
#include "coord.h"

namespace aw
{
	namespace game_mechanics
	{
		bool can_pass(const map::ptr &map, const coord &c, unit::ptr unit);
	}
}

#endif

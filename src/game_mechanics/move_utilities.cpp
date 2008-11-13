#include "move_utilities.h"

using namespace aw;

bool game_mechanics::can_pass(const map::ptr &map, const coord &c, unit::ptr unit)
{
	const unit::ptr o_unit(map->get_unit(c.x, c.y));
	if(o_unit)
	{
		if(unit->can_run_over())
			return true;

		if(o_unit->is_dummy())
			return true;

		if(o_unit->color() != unit->color())
			return false;
	}

	return true;
}

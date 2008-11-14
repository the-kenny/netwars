#include "supply_mechanics.h"

using namespace aw;

bool game_mechanics::can_repair(const map::ptr &map, const coord &unit_c, int funds)
{
	const unit::ptr unit = map->get_unit(unit_c.x, unit_c.y);

	assert(unit != NULL);

	if(!unit->can_repair())
		return false;

	if(get_repair_coordinates(map, unit_c, funds).empty())
		return false;

	return true;
}

bool game_mechanics::can_supply(const map::ptr &map, const coord &unit_c)
{
	const unit::ptr unit = map->get_unit(unit_c.x, unit_c.y);

	assert(unit != NULL);

	if(!unit->can_supply())
		return false;

	if(get_supply_coordinates(map, unit_c).empty())
		return false;

	return true;
}

bool game_mechanics::can_repair(const map::ptr &map, const coord &unit_c, const coord &target_c, int funds)
{
	const unit::ptr unit = map->get_unit(unit_c.x, unit_c.y);
	const unit::ptr target = map->get_unit(target_c.x, target_c.y);

	assert(unit != NULL);
	assert(target != NULL);

	const int price = unit_loader::instance().get_unit_info(target->get_name()).price/10;

	if(unit->color() == target->color() && unit->can_repair(target->get_environment()) && target->get_hp() < target->max_hp() && price <= funds)
		return true;
	else
		return false;
}

bool game_mechanics::can_supply(const map::ptr &map, const coord &unit_c, const coord &target_c)
{
	const unit::ptr unit = map->get_unit(unit_c.x, unit_c.y);
	const unit::ptr target = map->get_unit(target_c.x, target_c.y);

	assert(unit != NULL);
	assert(target != NULL);

	if(unit->color() == target->color() && unit->can_supply(target->get_environment()))
		return true;
	else
		return false;
}

aw::area game_mechanics::get_supply_coordinates(const map::ptr &map, const coord &unit_c)
{
	const unit::ptr unit = map->get_unit(unit_c.x, unit_c.y);

	assert(unit != NULL);

	area a = area::create(unit_c, 1);
	a.erase(unit_c);

	area ret;

	BOOST_FOREACH(const coord &c, a)
	{
		if(map->on_map(c.x, c.y) && map->get_unit(c.x, c.y))
		{
			if(can_supply(map, unit_c, c))
				ret.insert(c);
		}
	}

	return ret;
}

aw::area game_mechanics::get_repair_coordinates(const map::ptr &map, const coord &unit_c, int funds)
{
	const unit::ptr unit = map->get_unit(unit_c.x, unit_c.y);

	assert(unit != NULL);

	area a = area::create(unit_c, 1);
	a.erase(unit_c);

	area ret;

	BOOST_FOREACH(const coord &c, a)
	{
		if(map->on_map(c.x, c.y) && map->get_unit(c.x, c.y))
		{
			if(can_repair(map, unit_c, c, funds))
				ret.insert(c);
		}
	}

	return ret;
}



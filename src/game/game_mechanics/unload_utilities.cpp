#include "unload_utilities.h"
#include "../units/unit_base.h"
#include "traverse.h"

#include <boost/smart_ptr.hpp>
#include <boost/foreach.hpp>

using namespace aw;

bool game_mechanics::can_unload(const map::ptr &map, const coord &transporter_c)
{
	transporter::ptr t = boost::dynamic_pointer_cast<transporter>(map->get_unit(transporter_c.x, transporter_c.y));
	assert(t != NULL);

	if(!t->can_unload())
		return false;

	for(std::size_t i = 0; i < t->loaded_units_count(); i++)
	{
		if(!get_unload_coordinates(map, transporter_c, i).empty())
			return true;
	}

	return false;
}

bool game_mechanics::can_unload(const map::ptr &map, const coord &transporter_c, int index)
{
	transporter::ptr t = boost::dynamic_pointer_cast<transporter>(map->get_unit(transporter_c.x, transporter_c.y));
	assert(t != NULL);

	if(!t->can_unload())
		return false;

	if(!get_unload_coordinates(map, transporter_c, index).empty())
		return true;
	else
		return false;
}

area game_mechanics::get_unload_coordinates(const map::ptr &map, const coord &transporter_c, int index)
{
	transporter::ptr t = boost::dynamic_pointer_cast<transporter>(map->get_unit(transporter_c.x, transporter_c.y));
	assert(t != NULL);
	assert(t->can_unload() == true);

	unit::ptr unit = t->get_unit(index);
	assert(unit != NULL);

	game_mechanics::traverse trav;
	trav.calculate(map, transporter_c, unit, 1);

	area a(trav.get_coordinates());

	BOOST_FOREACH(const coord &c, a)
	{
		if(map->on_map(c.x, c.y) && map->get_unit(c.x, c.y) != NULL)
			a.erase(c);
	}

	return a;
}

const std::vector<std::size_t> game_mechanics::get_unloadable_indices(const map::ptr &map, const coord &transporter_c)
{
	transporter::ptr t = boost::dynamic_pointer_cast<transporter>(map->get_unit(transporter_c.x, transporter_c.y));
	assert(t != NULL);
	assert(t->can_unload() == true);

	std::vector<std::size_t> unloadable_indices;

	for(std::size_t i = 0; i < t->loaded_units_count(); i++)
	{
		if(!get_unload_coordinates(map, transporter_c, i).empty())
			unloadable_indices.push_back(i);
	}

	return unloadable_indices;
}

const std::vector<std::pair<std::size_t, unit::ptr> > game_mechanics::get_unloadable_units(const map::ptr &map, const coord &transporter_c)
{
	transporter::ptr t = boost::dynamic_pointer_cast<transporter>(map->get_unit(transporter_c.x, transporter_c.y));
	assert(t != NULL);
	assert(t->can_unload() == true);

	std::vector<std::pair<std::size_t, unit::ptr> > list;

	BOOST_FOREACH(std::size_t s, get_unloadable_indices(map, transporter_c))
		list.push_back(std::make_pair(s, t->get_unit(s)));

	return list;
}



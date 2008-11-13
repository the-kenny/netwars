#include "fire_range.h"
#include "traverse.h"

#include "attack_utilities.h"

#include <vector>

using namespace aw;
using namespace aw::game_mechanics;

void fire_range::calculate(const map::ptr &map, int x, int y)
{
	m_coordinates.clear();

	m_attacking_unit.assign(x, y);

	const unit::ptr u = map->get_unit(x, y);
	int fire_range = u->get_attack_range();

	if(fire_range == 1)
	{
		if(map->on_map(x+1, y))
			m_coordinates.push_back(coord(x+1, y));
		if(map->on_map(x-1, y))
			m_coordinates.push_back(coord(x-1, y));
		if(map->on_map(x, y-1))
			m_coordinates.push_back(coord(x, y-1));
		if(map->on_map(x, y+1))
			m_coordinates.push_back(coord(x, y+1));
	}
	else
	{
		const int distance = u->get_attack_distance();

//		std::cout << "[fire_range::calculate] Range: " << range << " Distance: " << distance << std::endl;

		int temp_range = 0;

		for(int x2 = -(fire_range+distance); x2 <= fire_range+distance; x2++)
		{
			for(int y2 = -temp_range; y2 <= temp_range; y2++)
			{
				if((x2-y2 > distance || y2-x2 > distance) || (x2+y2 < -distance || y2+x2 > distance))
				{
					coord c(x2+x, y2+y);

					if(map->on_map(c.x, c.y))
						m_coordinates.push_back(c);
				}
			}

			if(x2 < 0)
				temp_range++;
			else
				temp_range--;
		}
	}
}

void fire_range::print(std::ostream &o)
{
	o << "[fire_range::print]" << '\n';
	for(int y = 0; y < 20; y++)
	{
		for(int x = 0; x < 30; x++)
		{
			if(m_coordinates.contains(coord(x, y)))
				std::cout << "X";
			else
				std::cout << "  ";
		}
		std:: cout << "\n";
	}
}

const area &fire_range::get_victims(const map::ptr &map, const player::ptr &/*player*/)
{
	m_opposite_coords.clear();

	const int att_x = m_attacking_unit.x;
	const int att_y = m_attacking_unit.y;
	unit::ptr attacker = map->get_unit(att_x, att_y);

	assert(map->on_map(att_x, att_y));
	assert(attacker != NULL);

	BOOST_FOREACH(const coord &c, m_coordinates)
	{
		int def_x = c.x;
		int def_y = c.y;

		if(map->on_map(def_x, def_y))
		{
			unit::ptr victim = map->get_unit(def_x, def_y);

			if(victim && attack_utilities::can_attack(attacker, victim))
			{
				m_opposite_coords.push_back(c);
			}
		}
	}

	return m_opposite_coords;
}

void fire_range_preview::calculate(const map::ptr &map, int x, int y)
{
	const unit::ptr &u(map->get_unit(x, y));

	if(!u)
		throw std::runtime_error("No unit at " + boost::lexical_cast<std::string>(x) + "|" + boost::lexical_cast<std::string>(y));


	if(u->get_attack_distance() == 0)
	{
		traverse tr;
		tr.calculate(map,coord(x, y));

		area temp_coords;

		//Fill the 4 directions around the unit
		temp_coords.push_back(coord(x+1, y));
		temp_coords.push_back(coord(x-1, y));
		temp_coords.push_back(coord(x, y+1));
		temp_coords.push_back(coord(x, y-1));

		BOOST_FOREACH(const area::value_type &t, tr.get_coordinates())
		{
			if(map->on_map(t.x, t.y) && !map->get_unit(t.x, t.y))
			{
				temp_coords.push_back(coord(t.x+1, t.y));
				temp_coords.push_back(coord(t.x-1, t.y));
				temp_coords.push_back(coord(t.x, t.y+1));
				temp_coords.push_back(coord(t.x, t.y-1));
			}
		}

		m_area.assign(temp_coords.begin(), temp_coords.end());
	}
	else
	{
		fire_range f;
		f.calculate(map, x, y);

		m_area.assign(f.get_coordinates());
	}
}

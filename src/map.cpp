#include "map.h"
#include "player.h"
#include "game_mechanics/player_utilities.h"

#include <set>

#include <boost/array.hpp>

using namespace aw;

map::map()
{
	this->init();
}

map::map(const boost::multi_array<terrain::ptr, 2> &terrain, const boost::multi_array<unit::ptr, 2> &units)
{
	this->init();

	for(int x = 0; x < 30; x++)
	{
		for(int y = 0; y < 20; y++)
		{
			if(units[x][y])
				m_units[x][y] = units::create(units[x][y]->type(), units[x][y]->color());

//				m_terrain[x][y] = terrain::ptr(new aw::terrain(terrain[x][y]->type(), terrain[x][y]->extra()));
			m_terrain[x][y] = terrain::create(terrain[x][y]->type(), terrain[x][y]->extra());
		}
	}
}

void map::add_unit(unsigned int x, unsigned int y, const unit::ptr &u)
{
	if(m_units[x][y])
		throw std::runtime_error("Cant add unit, there is already one.");

	m_units[x][y] = u;
}

void map::delete_unit(unsigned int x, unsigned int y)
{
	m_units[x][y].reset();
}

unit::ptr map::detach_unit(unsigned int x, unsigned int y)
{
	unit::ptr p = m_units[x][y];
	m_units[x][y].reset();
	return p;
}

void map::move_unit(unsigned int from_x, unsigned int from_y, unsigned int to_x, unsigned int to_y)
{
	if(!m_units[from_x][from_y] || m_units[to_x][to_y])
		throw std::runtime_error("[map::move_unit] Kein Unit zu bewegen bzw. Zielposition bereits belegt");

	m_units[to_x][to_y] = m_units[from_x][from_y];
	m_units[from_x][from_y].reset();
}

void map::change_building_color(unsigned int x, unsigned int y, const player::ptr &player)
{
	terrain::ptr p = m_terrain[x][y];

	if(!on_map(x, y))
		throw std::runtime_error("[map::change_building_color] Gegebene Koordinaten sind nicht auf der Karte.");

	if(!p->is_building())
		throw std::runtime_error("[map::change_building_color] Auf gegebener Position ist kein Gebäude.");

//		if(p->capture_points() > 0)
//			std::clog << "[map::change_building_color] WARNUNG: Ändere die Farbe eines Gebäudes dessen Capture-Points > 0 sind." << std::endl;

	std::clog << "[map::change_building_color] Übernehme Gebäude" << std::endl;
	p->extra(player->get_building_color());
}

//	unsigned int map::get_player_count()
//	{
//		unsigned int buildings = 0;
//		unsigned int units = 0;
//
//		std::set<terrain::extras> terrain;
//		std::set<unit::colors> unit;
//
//		for(int x = 0; x < 30; x++)
//		{
//			for(int y = 0; y < 20; y++)
//			{
//				if(terrain::is_building(m_terrain[x][y]->type()) && m_terrain[x][y]->extra() != terrain::WHITE)
//				{
//					if(terrain.insert(m_terrain[x][y]->extra()).second)
//						++buildings;
//				}
//
//				if(m_units[x][y])
//				{
//					if(unit.insert(m_units[x][y]->color()).second)
//						++units;
//				}
//			}
//		}
//
//		return std::max(units, buildings);
//	}

//	bool map::participates(player::colors color)
//	{
//		player p(color);
//
//		for(int x = 0; x < 30; x++)
//		{
//			for(int y = 0; y < 20; y++)
//			{
//				if((terrain::is_building(m_terrain[x][y]->type()) && m_terrain[x][y]->extra() != terrain::WHITE && p.his_building((m_terrain[x][y]))) || (m_units[x][y] && p.his_unit(m_units[x][y])))
//					return true;
//			}
//		}
//
//		return false;
//	}

display::scene::ptr map::scene()
{
	aw::display::scene::ptr s(new aw::display::scene);

	for(int x = 0; x < 30; x++)
		for(int y = 0; y < 20; y++)
			{
//					s->terrain_type(x, y, m_terrain[x][y]->type());
//					s->terrain_extra(x, y, m_terrain[x][y]->extra());

				s->set_terrain(x, y, m_terrain[x][y]);
				s->set_unit(x, y, m_units[x][y]);
			}

	return s;
}

unsigned int map::num_buildings(player::colors c)
{
	unsigned int count = 0;
	player p(c);

	for(int x = 0; x < 30; x++)
	{
		for(int y = 0; y < 20; y++)
		{
			if(m_terrain[x][y]->is_building() && (m_terrain[x][y]->extra() != terrain::WHITE && m_terrain[x][y]->extra() != terrain::LAUNCHED) && p.his_building(m_terrain[x][y]))
				count++;
		}
	}

	return count;
}

bool map::valid()
{
	bool participators[5] = { false };
	participators[0] = game_mechanics::participates(shared_from_this(), player::RED);
	participators[1] = game_mechanics::participates(shared_from_this(), player::BLUE);
	participators[2] = game_mechanics::participates(shared_from_this(), player::GREEN);
	participators[3] = game_mechanics::participates(shared_from_this(), player::YELLOW);
	participators[4] = game_mechanics::participates(shared_from_this(), player::BLACK);

	if(std::count(participators, participators+5, true) < 2)
		return false;
	else
		return true;
}

void map::init()
{
	typedef boost::multi_array<terrain::ptr, 2> terrain_array_t;
	typedef boost::multi_array<unit::ptr, 2> unit_array_t;

	boost::array<unit_array_t::index, 2> ua;
	boost::array<terrain_array_t::index, 2> ta;

	ua[0] = ta[0] = this->width();
	ua[1] = ta[1] = this->height();

	m_terrain.resize(ta);
	m_units.resize(ua);
}


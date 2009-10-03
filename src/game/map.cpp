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

map::map(const boost::multi_array<terrain::ptr, 2> &terrain, 
		 const boost::multi_array<unit::ptr, 2> &units)
{
  this->init();
  this->fill_arrays(terrain, units);
}

map::map(const map_loader::loaded_map::ptr& lmap) {
  this->init();
  this->fill_arrays(lmap->terrain, lmap->unit);

  title = lmap->title;
  author = lmap->author;
  description = lmap->description;
}

void map::add_unit(const coord& c, const unit::ptr &u)
{
	if(m_units[c.x][c.y])
		throw std::runtime_error("Cant add unit, there is already one.");

	m_units[c.x][c.y] = u;
}

void map::delete_unit(const coord& c)
{
	m_units[c.x][c.y].reset();
}

unit::ptr map::detach_unit(const coord& c)
{
	unit::ptr p = m_units[c.x][c.y];
	m_units[c.x][c.y].reset();
	return p;
}

void map::move_unit(const coord& from, const coord& to)
{
	if(!m_units[from.x][from.y] || m_units[to.x][to.y])
		throw std::runtime_error("[map::move_unit] Kein Unit zu bewegen bzw. Zielposition bereits belegt");

	m_units[to.x][to.y] = m_units[from.x][from.y];
	m_units[from.x][from.y].reset();
}

void map::change_building_color(const coord& c, const player::ptr &player)
{
	terrain::ptr p = m_terrain[c.x][c.y];

	if(!on_map(c))
		throw std::runtime_error("[map::change_building_color] Gegebene Koordinaten sind nicht auf der Karte.");

	if(!p->is_building())
		throw std::runtime_error("[map::change_building_color] Auf gegebener Position ist kein Gebäude.");

//		if(p->capture_points() > 0)
//			std::clog << "[map::change_building_color] WARNUNG: Ändere die Farbe eines Gebäudes dessen Capture-Points > 0 sind." << std::endl;

	std::clog << "[map::change_building_color] Übernehme Gebäude" << std::endl;
	p->extra(player->get_building_color());
	p->reset_capture_points();
}

void map::neutralize_building(const coord& c) {
  assert(on_map(c));
  
  terrain::ptr p = m_terrain[c.x][c.y];
  
  assert(p->is_building());
  
  std::clog << "Changing color of building at " << c 
			<< " to white" << std::endl;
  p->extra(terrain::WHITE);
  p->reset_capture_points();
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
	participators[0] = game_mechanics::participates(shared_from_this(),
													player::RED);
	participators[1] = game_mechanics::participates(shared_from_this(), 
													player::BLUE);
	participators[2] = game_mechanics::participates(shared_from_this(), 
													player::GREEN);
	participators[3] = game_mechanics::participates(shared_from_this(), 
													player::YELLOW);
	participators[4] = game_mechanics::participates(shared_from_this(), 
													player::BLACK);

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

void map::fill_arrays(const boost::multi_array<terrain::ptr, 2> &terrain, 
					  const boost::multi_array<unit::ptr, 2> &units) {
  this->init();
  
  for(int x = 0; x < 30; x++) {
	for(int y = 0; y < 20; y++) {
	  if(units[x][y])
		m_units[x][y] = units::create(units[x][y]->type(), 
									  units[x][y]->color());
	  
	  m_terrain[x][y] = terrain::create(terrain[x][y]->type(), 
										terrain[x][y]->extra());
	}
  }
}

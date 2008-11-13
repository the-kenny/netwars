#include "terrain.h"

using namespace aw;

terrain::terrain(terrain::types type, terrain::extras extra)
: m_type(type), m_extra(extra)
{
	//TODO: After a change, the defense has to be new calculated
	switch(type)
	{
		case terrain::PLAIN:
		case terrain::REEF:
			m_defense = 1;
			break;
		case terrain::FOREST:
			m_defense = 2;
			break;
		case terrain::CITY:
		case terrain::BASE:
		case terrain::AIRPORT:
		case terrain::PORT:
		case terrain::LAB:
			m_defense = 3;
			break;
		case terrain::HEADQUARTER:
		case terrain::MOUNTAIN:
			m_defense = (extra == WHITE) ? 3 : 4; //Silo/HQ
			break;

		default:
			m_defense = 0;
	}
}

bool terrain::is_building(types t, extras e)
{
	switch(t)
	{
		case terrain::HEADQUARTER:
			if(e == WHITE || e == LAUNCHED) return false; // A silo isn't a building
		case terrain::CITY:
		case terrain::BASE:
		case terrain::AIRPORT:
		case terrain::PORT:
		case terrain::TOWER:
		case terrain::LAB:
			return true;
			break;

		default:
			return false;
			break;
	}
}

bool terrain::is_vulnerable(types t)
{
	switch(t)
	{
		case terrain::SEGMENT_PIPE:
			return true;
			break;

		default:
			return false;
			break;
	}
}

bool terrain::is_water(types t)
{
	switch(t)
	{
		case WATER:
		case REEF:
		//case terrain::RIVER: //Ein Fluss zählt als fester Boden
		case BRIDGE: //Muss so sein (Klippen an Brücken)
		case BEACH:
			return true;
			break;

		default:
			return false;
			break;
	}
}

int terrain::movement_costs(unit::move_types t) const
{
	int ret = -1;

	//move[] 0=infantry, 1=mech, 2=tread, 3=tires, 4=air, 5=sea, 6=transport, 7=oozium, 8=pipe, 9=hover

	//{ //ugly //NOTE(#1): Maybe replace with std::bitset
	static const int plain_a[] = { 1, 1, 1, 2, 1, -1, -1, 1, -1 };
	static const int forest_a[] = { 1, 1, 2, 3, 1, -1, -1, 1, -1 };
	static const int building_a[] = { 1, 1, 1, 1, 1, -1, -1, 1, -1 };
	static const int bridge_a[] = { 1, 1, 1, 1, 1, -1, -1, 1, -1 };
	static const int hq_a[] = { 1, 1, 1, 1, 1, -1, -1, 1, -1 };
	static const int mountain_a[] = { 2, 1, -1, -1, 1, -1, -1, 1, -1 };
	static const int pipe_a[] = { -1, -1, -1, -1, -1, -1, -1, -1,1  };
	static const int reef_a[] = { -1, -1, -1, -1, 1, 2, 2, -1, -1, -1 };
	static const int street_a[] = { 1, 1, 1, 1, 1, -1, -1, 1, -1 };
	static const int water_a[] = { -1, -1, -1, -1, 1, 1, 1, -1, -1 };
	static const int silo_a[] = { 1, 1, 1, 1, 1, -1, -1, 1, -1 };
	static const int river_a[] = { 2, 1, -1, -1, 1, -1, -1, 1, -1, -1 };
	static const int beach_a[] = { 1, 1, 1, 1, 1, -1, 1, 1, -1, -1 };
	static const int port_a[] = { 1, 1, 1, 1, 1, 1, 1, 1, -1, -1 };
	//}

	switch(m_type)
	{
		case PLAIN:
		case WRECKAGE:
			ret = plain_a[t];
			break;
		case RIVER:
			ret = river_a[t];
			break;
		case MOUNTAIN:
			ret = mountain_a[t];
			break;
		case FOREST:
			ret = forest_a[t];
			break;
		case STREET:
			ret = street_a[t];
			break;
		case BRIDGE:
			ret = bridge_a[t];
			break;
		case PIPE:
		case SEGMENT_PIPE:
			ret = pipe_a[t];
			break;
		case WATER:
		case SEASIDE:
			ret = water_a[t];
			break;
		case REEF:
			ret = reef_a[t];
			break;
		case BEACH:
			ret = beach_a[t];
			break;
		case HEADQUARTER:
			if(m_extra == WHITE || m_extra == LAUNCHED)
				ret = silo_a[t];
			else
				ret = hq_a[t];
			break;
		case CITY:
		case BASE:
		case AIRPORT:
		case TOWER:
		case LAB:
			ret = building_a[t];
			break;
		case PORT:
			ret = port_a[t];
			break;
	}

	return ret;
}

std::string terrain::get_name(terrain::types t)
{
	switch(t)
	{
		case PLAIN:
			return "plain";
		case RIVER:
			return "river";
		case MOUNTAIN:
			return "mountain";
		case FOREST:
			return "forest";
		case STREET:
			return "street";
		case BRIDGE:
			return "bridge";
		case PIPE:
			return "pipe";
		case SEGMENT_PIPE:
			return "segment_pipe";
		case WATER:
			return "water";
		case REEF:
			return "reef";
		case SEASIDE:
			return "seaside";
		case BEACH:
			return "beach";
		case WRECKAGE:
			return "wreckage";
		case HEADQUARTER:
			return "headquarter";
		case CITY:
			return "city";
		case BASE:
			return "base";
		case AIRPORT:
			return "airport";
		case PORT:
			return "port";
		case TOWER:
			return "tower";
		case LAB:
			return "lab";

		default:
			throw std::runtime_error("[terrain::get_name] Unsupported type");
	}
}

silo::silo()
: terrain(HEADQUARTER, WHITE), m_fired(false), m_range(2), m_damage(3) //A white HQ is a silo
{
}

void silo::fire()
{
	m_fired = true;
	this->extra(terrain::LAUNCHED);
}

building::building(terrain::types type, terrain::extras extra)
: terrain(type, extra)
{
	m_capture_points = 20;
	m_repair_points = 2;

	switch(this->type())
	{
		case HEADQUARTER:
		case CITY:
		case BASE:
			m_can_supply = true;
			m_supply_environment = unit::GROUND;
			break;
		case AIRPORT:
			m_can_supply = true;
			m_supply_environment = unit::AIR;
			break;
		case PORT:
			m_can_supply = true;
			m_supply_environment = unit::WATER;
			break;

		default:
			m_can_supply = false;
			break;
	}
}

int building::capture_points()
{
	if(!is_building(this->type()))
		throw std::runtime_error("Terrain ist kein Gebäude!");

	return m_capture_points;
}

void building::reset_capture_points(int points)
{
	m_capture_points = points;
}

unsigned int building::capture(unsigned int points)
{
	m_capture_points -= points;

	std::clog << "[terrain::capture] Restliche Capture-Points: " << m_capture_points << std::endl;

	if(m_capture_points < 0)
		m_capture_points = 0;

	return m_capture_points;
}

bool building::can_supply(unit::environments e) const
{
	return (this->can_supply() && e == this->get_supply_environment());
}

terrain::ptr terrain::create(terrain::types t, terrain::extras e)
{
	switch(t)
	{
		case HEADQUARTER:
			if(t == HEADQUARTER && (e == WHITE || e == LAUNCHED)) { return terrain::ptr(new silo); break; }
		case CITY:
		case BASE:
		case AIRPORT:
		case PORT:
		case TOWER:
		case LAB:
			return terrain::ptr(new building(t, e));

		default:
			return terrain::ptr(new terrain(t, e));
	};
}

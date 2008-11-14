#ifndef AW_TERRAIN_H
	#define AW_TERRAIN_H

#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <iostream>

#include "units/unit_base.h"

namespace aw
{
	//TODO: change_type(types new_type) einbauen und nutzen
	class terrain
	{
		public:
			typedef boost::shared_ptr<terrain> ptr;

			enum types { PLAIN, RIVER, MOUNTAIN, FOREST, STREET, BRIDGE, PIPE, SEGMENT_PIPE, WATER, REEF, SEASIDE, BEACH, WRECKAGE, HEADQUARTER, CITY, BASE, AIRPORT, PORT, TOWER, LAB };
			enum directions { UNDEF, U, L, D, R, DR, LD, LDR, UL, ULD, ULDR, UD, ULR, UR, UDR, LR };
			enum extras { NONE, CORNER, SMALL, BIG, MOUTH, RED, BLUE, GREEN, YELLOW, BLACK, WHITE, LAUNCHED };

			static terrain::ptr create(terrain::types t = WATER, terrain::extras e = NONE);

			terrain(terrain::types type, terrain::extras extra);
			virtual ~terrain()
			{
			}

			types type() const { return m_type; }
			void type(types new_type) { m_type = new_type; }

			extras extra() const { return m_extra; }
			void extra(extras new_extra) { m_extra = new_extra; }

			unsigned int defense_value() { return m_defense; }

			bool is_building() const { return is_building(this->type(), this->extra()); }
			bool is_vulnerable() const { return is_vulnerable(this->type()); }
			bool is_ground() const { return is_ground(this->type()); }
			bool is_water() const { return is_water(this->type()); }

			//e is provided to check if it isn't a silo (white HQ)
			bool static is_building(types t, extras e = NONE);
			bool static is_vulnerable(types t);
			bool static is_ground(types t) { return (!is_water(t)); }
			bool static is_water(types t);

			//Returns -1 if impassible
			int movement_costs(unit::move_types t) const;
			bool can_pass(unit::move_types t) const { return this->movement_costs(t) != -1; }

			static std::string get_name(terrain::types t);
			std::string get_name() const { return get_name(this->m_type); }

		private:
			terrain::types m_type;
			extras m_extra;
			unsigned int m_defense;
	};

	class silo: public terrain
	{
		public:
			typedef boost::shared_ptr<silo> ptr;

			silo();

			bool has_fired() const { return m_fired; }
			void fire();

			int get_range() const { return m_range; }
			int get_damage() const { return m_damage; }

		private:
			bool m_fired;
			int m_range;
			int m_damage;
	};

	//TODO: change_color(color new_color) einbauen und nutzen
	class building: public terrain
	{
		public:
			typedef boost::shared_ptr<building> ptr;

			building(terrain::types type, terrain::extras extra);

			int capture_points();
			void reset_capture_points(int points = 20);
			unsigned int capture(unsigned int points = 10);

			bool can_supply() const { return m_can_supply; }
			bool can_supply(unit::environments e) const;

			unit::environments get_supply_environment() const { return m_supply_environment; }
			unsigned int get_repair_points() const { return m_repair_points; }

		private:
			int m_capture_points;

			bool m_can_supply;
			unit::environments m_supply_environment;
			unsigned int m_repair_points;
	};
}

#endif

#ifndef AW_DAMAGE_CALC_H
	#define AW_DAMAGE_CALC_H

#include "game/units/unit_base.h"
#include "game/map.h"
#include "game/coord.h"

namespace aw
{
	namespace game_mechanics
	{
		//TODO: Create a way to get the unfloored/unceiled damage for dummy_units
		class damage_calc
		{
			public:
//				typedef std::pair<int, int> coord_t;

				void calculate(const map::ptr &map, coord att_coord, coord def_coord);

				int get_unit1_life() const { return m_unit1_life; }
				int get_unit2_life() const { return m_unit2_life; }

				float get_attacker_damage() const { return m_attacker_damage; }
				float get_victim_damage() const { return m_victim_damage; }

			private:
				int m_unit1_life;
				int m_unit2_life;

				float m_attacker_damage;
				float m_victim_damage;
		};
	}
}

#endif

#ifndef AW_EXPLOSION_DAMAGE_H
	#define AW_EXPLOSION_DAMAGE_H

#include "../coord.h"
#include "../area.h"
#include "../map.h"

#include <map>

namespace aw
{
	namespace game_mechanics
	{
		class explosion_damage
		{
			public:
				typedef std::pair<coord, float> value_type;
				typedef std::map<coord, float>::iterator iterator;
				typedef std::map<coord, float>::const_iterator const_iterator;

				iterator begin() { return m_calculated_hp.begin(); }
				iterator end() { return m_calculated_hp.end(); }

				const_iterator begin() const { return m_calculated_hp.begin(); }
				const_iterator end() const { return m_calculated_hp.end(); }

				const area &get_area() const { return m_area; }

				float get_hp(const coord &c);
				void calculate(const map::ptr &map, const coord &c, int range, int damage);

			private:
				aw::area m_area;
				std::map<coord, float> m_calculated_hp;
		};
	}
}

#endif

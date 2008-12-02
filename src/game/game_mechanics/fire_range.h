#ifndef AW_FIRE_RANGE_H
	#define AW_FIRE_RANGE_H


#include "../coord.h"
#include "../area.h"
#include "../map.h"


namespace aw
{
	namespace game_mechanics
	{
		class fire_range
		{
			public:
				void calculate(const map::ptr &map, const coord& c);

				void print(std::ostream &o);
				const area &get_coordinates() const { return m_coordinates; }
				const area &get_victims(const map::ptr &map, const player::ptr &player);

			private:
				area m_coordinates;
				area  m_opposite_coords;

				coord m_attacking_unit_c;
		};

		class fire_range_preview
		{
			public:
				void calculate(const map::ptr &map, int x, int y);
				const area &get_coordinates() const { return m_area; }

			private:
				area m_area;
		};
	}
}

#endif

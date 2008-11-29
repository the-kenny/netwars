#ifndef AW_TRAVERSE_H
	#define AW_TRAVERSE_H

#include "../map.h"
#include "../units/unit_base.h"
#include "../terrain.h"
#include "game/coord.h"
#include "game/area.h"

#include <vector>
#include <map>

#include <ostream>

namespace aw
{
	namespace game_mechanics
	{
		class traverse
		{
			public:
//				typedef std::pair<int, int> coord_t;
				typedef boost::shared_ptr<traverse> ptr;
				typedef aw::area area_type;

				void calculate(const map::ptr &map, const coord &origin);
				void calculate(const map::ptr &map, const coord &origin, const unit::ptr &u, int range = -1);

				inline bool in_range(const coord &pos) const { return this->get_rest_mp(pos.x, pos.y) != -1; }
				inline coord get_origin() const { return m_start; }

				//const std::vector &get_possibilities(const unit_t::ptr &unit);
				const area_type &get_coordinates() const;
				int get_rest_mp(int x, int y) const;

				void reset()
				{
					m_coords.clear();
					m_move_range_left.clear();
				}

				void print(std::ostream &o) const;

			private:
//				void move(int x, int y, int dir, int rest_movement_range, int rest_gas, bool left, bool right, unit::ptr u, terrain::types previous_terrain);
				void move(int x, int y, int dir, int rest_movement_range, int rest_gas, bool left, bool right, const unit::ptr &u);
//				bool can_overdrive(int x, int y, unit::ptr unit) const;

				coord m_start;
				area_type m_coords;
				std::map<coord, int> m_move_range_left;

				map::ptr m_map;
		};

		std::ostream &operator<<(std::ostream &o, const traverse &t);
	}
}

#endif

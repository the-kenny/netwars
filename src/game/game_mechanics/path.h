#ifndef AW_PATH_H
	#define AW_PATH_H

#include "game/game_mechanics/traverse.h"
#include "game/coord.h"

#include <boost/shared_ptr.hpp>

#include <ostream>

namespace aw
{
	namespace game_mechanics
	{
		class path
		{
			public:
//				typedef std::pair<int, int> coord_t;
				typedef boost::shared_ptr<path> ptr;
				typedef aw::area path_t;

				path(const coord &start = coord(), const coord &end = coord());

				const coord &end() const { return m_end; }
				const coord &start() const { return m_start; }
				const path_t &get_path() const { return m_path; }

				void remove_element(const coord &c);

				int get_fuel_costs() const { return m_fuel_costs; }
				int get_length() const { return m_path.size(); }

				void reset();

				void print(std::ostream &o) const;

				void calculate(const map::ptr &map, const traverse::ptr &t, const coord &start, const coord &end);

			private:
				void calculate(const map::ptr &map, const traverse::ptr &t, const coord &start, const coord &end, const unit::ptr &unit, int fuel = -1);

				bool move(int x, int y, int end_x, int end_y, int dir, int rest_movement_range, int rest_gas, bool left, bool right, const unit::ptr &u);
				void append(int x, int y);

				void recalculate_costs(const unit::ptr &u);
				void remove_doubles();

				bool m_recalculate;

				map::ptr m_map;
				traverse::ptr m_traverse;

				coord m_start;
				coord m_end;

				path_t m_path;
				int m_fuel_costs;
		};
	}
}

#endif

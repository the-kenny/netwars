#ifndef AW_MAP_H
	#define AW_MAP_H

#include "units/units.h"
#include "terrain.h"
#include "player.h"
#include "coord.h"

#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace aw
{
	//TODO(#1): make variable-sized maps work
	class map: public boost::enable_shared_from_this<map>
	{
		public:
			typedef boost::shared_ptr<map> ptr;

			map();
			map(const boost::multi_array<terrain::ptr, 2> &terrain, const boost::multi_array<unit::ptr, 2> &units);

			const unit::ptr &get_unit(const coord& c) const { return m_units[c.x][c.y]; }
			const terrain::ptr &get_terrain(const coord& c) const { return m_terrain[c.x][c.y]; }

			void add_unit(const coord& c, const unit::ptr &u);
			void delete_unit(const coord& c);
			unit::ptr detach_unit(const coord& c);

			void move_unit(const coord& from, const coord& to);
			void change_building_color(const coord& c, const player::ptr &player);

			unsigned int num_buildings(player::colors c);

			bool valid();

			int width() const { return 30; }
			int height() const { return 20; }

			bool on_map(const coord& c) const
			{
				return (c.x < 30 && c.x > -1 && c.y < 20 && c.y > -1);
			}
			
		private:
			void init();

			boost::multi_array<terrain::ptr, 2> m_terrain;
			boost::multi_array<unit::ptr, 2> m_units;
	};
}

#endif

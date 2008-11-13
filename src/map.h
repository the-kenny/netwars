#ifndef AW_MAP_H
	#define AW_MAP_H

#include "units/units.h"
#include "terrain.h"
#include "player.h"

#include "graphics/scene.h"

#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace aw
{
	//TODO(#2): Make iterable
	//TODO(#1): make variable-sized maps work
	//TODO(#2): Use the coord-class
	class map: public boost::enable_shared_from_this<map>
	{
		public:
			typedef boost::shared_ptr<map> ptr;

			map();
			map(const boost::multi_array<terrain::ptr, 2> &terrain, const boost::multi_array<unit::ptr, 2> &units);

			const unit::ptr &get_unit(unsigned int x, unsigned int y) const { return m_units[x][y]; }
			const terrain::ptr &get_terrain(unsigned int x, unsigned int y) const { return m_terrain[x][y]; }

			void add_unit(unsigned int x, unsigned int y, const unit::ptr &u);
			void delete_unit(unsigned int x, unsigned int y);
			unit::ptr detach_unit(unsigned int x, unsigned int y);

			void move_unit(unsigned int from_x, unsigned int from_y, unsigned int to_x, unsigned int to_y);
			void change_building_color(unsigned int x, unsigned int y, const player::ptr &player);

//			unsigned int get_player_count(); //Haesslich implentiert
//			bool participates(player::colors color); //Nun im namespace game_mechanics::participates()
			unsigned int num_buildings(player::colors c);

			bool valid();

			int width() const { return 30; }
			int height() const { return 20; }

			bool on_map(int x, int y) const
			{
				return (x < 30 && x > -1 && y < 20 && y > -1);
			}

			display::scene::ptr scene();

		private:
			void init();

			boost::multi_array<terrain::ptr, 2> m_terrain;
			boost::multi_array<unit::ptr, 2> m_units;
	};
}

#endif

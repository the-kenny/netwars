#ifndef AW_GRAPHICS_SCENE_H
	#define AW_GRAPHICS_SCENE_H

#include <boost/shared_ptr.hpp>
#include <boost/multi_array.hpp>

#include "area.h"
#include "coord.h"
#include "terrain.h"
#include "units/unit_base.h"
#include "map.h"

namespace aw
{
	class scene
	{
		public:
			typedef boost::shared_ptr<scene> ptr;

			scene(const map::ptr &map)
			: m_terrains(boost::extents[30][20]), m_units(boost::extents[30][20])
			{
				for(int x = 0; x < 30; x++)
					for(int y = 0; y < 20; y++)
					{
						const coord c(x, y);
						this->set_terrain(c, map->get_terrain(c));
						this->set_unit(c, map->get_unit(c));
					}
			}

			scene()
			: m_terrains(boost::extents[30][20]), m_units(boost::extents[30][20])
			{
				for(int x = 0; x < 30; x++)
				{
					for(int y = 0; y < 20; y++)
					{
						m_terrains[x][y] = terrain::create();
					}
				}
			}

			const terrain::ptr &get_terrain(const coord& c) const { return get_terrain(c.x, c.y); }
			const terrain::ptr &get_terrain(int x, int y) const { return m_terrains[x][y]; }
			void set_terrain(const coord& c, const terrain::ptr &terr) { m_terrains[c.x][c.y] = terr; }

			const unit::ptr &get_unit(const coord& c) const { return get_unit(c.x, c.y); }
			const unit::ptr &get_unit(int x, int y) const { return m_units[x][y]; }
			void set_unit(const coord& c, const unit::ptr &u) { m_units[c.x][c.y] = u; }

			bool is_highlighted(const coord& c) { return m_highlights.contains(c);}
			void set_highlighted_area(const aw::area &h) { m_highlights.assign(h); }
			const aw::area &get_highlighted_area() const { return m_highlights; }

			bool is_path(const coord& c) { return  m_path.contains(c);}
			void set_path(const aw::area &path) { m_path.assign(path); }
			const aw::area& get_path_area() const { return m_path;}

			const coord& highlight() const { return m_highlighted_unit; }
			void set_highlight(const coord &c) { m_highlighted_unit = c; }

		private:
			boost::multi_array<terrain::ptr, 2> m_terrains;
			boost::multi_array<unit::ptr, 2> m_units;

			aw::area m_highlights;
			aw::area m_path;
			aw::coord m_highlighted_unit;
	};
}

#endif

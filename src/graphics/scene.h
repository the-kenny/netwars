#ifndef AW_GRAPHICS_SCENE_H
	#define AW_GRAPHICS_SCENE_H

#include <boost/shared_ptr.hpp>
#include <boost/multi_array.hpp>

#include "../area.h"
#include "../coord.h"
#include "../terrain.h"
#include "../units/unit_base.h"

namespace aw
{
	namespace display
	{
		class scene
		{
			public:
				typedef boost::shared_ptr<scene> ptr;

				scene()
				: m_terrains(boost::extents[30][20]), m_units(boost::extents[30][20]) /*, m_highlights(boost::extents[30][20]), m_path(boost::extents[30][20])*/
				{
					for(int x = 0; x < 30; x++)
					{
						for(int y = 0; y < 20; y++)
						{
//							m_terrains[x][y] = terrain::ptr(new terrain);
							m_terrains[x][y] = terrain::create();
//							m_highlights[x][y] = false;
//							m_path[x][y] = false;
						}
					}
				}

//				terrain::types terrain_type(unsigned int x, unsigned int y) { return m_terrains[x][y]->type(); }
//				terrain::extras terrain_extra(unsigned int x, unsigned int y) { return m_terrains[x][y]->extra(); }

//				void terrain_type(unsigned int x, unsigned int y, terrain::types type) { m_terrains[x][y]->type(type); }
//				void terrain_extra(unsigned int x, unsigned int y, terrain::extras extra) { m_terrains[x][y]->extra(extra); }


				const terrain::ptr &get_terrain(unsigned int x, unsigned int y) const { return m_terrains[x][y]; }
				terrain::ptr &get_terrain(unsigned int x, unsigned int y) { return m_terrains[x][y]; }
				void set_terrain(unsigned int x, unsigned int y, const terrain::ptr &terr) { m_terrains[x][y] = terr; }

				const unit::ptr &get_unit(unsigned int x, unsigned int y) const { return m_units[x][y]; }
				void set_unit(unsigned int x, unsigned int y, const unit::ptr &u) { m_units[x][y] = u; }

				bool highlighted(unsigned int x, unsigned int y)
				{
					return m_highlights.contains(coord(x, y));
				}

				void highlighted(const aw::area &h) { m_highlights.assign(h); }

				void highlighted(unsigned int x, unsigned int y, bool highlight)
				{
					if(highlight && !m_highlights.contains(coord(x, y)))
						m_highlights.push_back(coord(x, y));
					else
						m_highlights.erase(coord(x, y));
				}

//				bool path(unsigned int x, unsigned int y) { return m_path[x][y]; }
//				void path(unsigned int x, unsigned int y, bool highlight) { m_path[x][y] = highlight; }

				bool path(unsigned int x, unsigned int y)
				{
					return  m_path.contains(coord(x, y));
				}

				void path(const aw::area &path) { m_path.assign(path); }

				void path(unsigned int x, unsigned int y, bool highlight)
				{
					if(highlight && !m_path.contains(coord(x, y)))
						m_path.push_back(coord(x, y));
					else if(!highlight)
						m_path.erase(coord(x, y));
				}

				coord highlighted_unit() const { return m_highlighted_unit; }
				void highlighted_unit(const coord &c) { m_highlighted_unit = c; }

			private:
				boost::multi_array<terrain::ptr, 2> m_terrains;
				boost::multi_array<unit::ptr, 2> m_units;

				aw::area m_highlights;
				aw::area m_path;
//				boost::multi_array<bool, 2> m_path;
				aw::coord m_highlighted_unit;
		};
	}
}

#endif

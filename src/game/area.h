#ifndef AREA_H
#define AREA_H

#include <algorithm>
#include <set>
#include <boost/foreach.hpp>

#include "game/coord.h"

namespace aw
{
	class area
	{
		public:
			typedef coord value_type;
			typedef std::set<value_type> container_t;
			typedef container_t::iterator iterator;
			typedef container_t::const_iterator const_iterator;

			area(const container_t &coords)
			: m_coords(coords) {}
			area() {}
			~area() {}

			void assign(iterator begin, iterator end)
			{
				this->clear();
				m_coords.insert(begin, end);
			}

			template<class T>
			void assign(const T &c)
			{
				this->assign(c.begin(), c.end());
			}
			
			iterator begin() { return m_coords.begin(); }
			iterator end() { return m_coords.end(); }

			const_iterator begin() const { return m_coords.begin(); }
			const_iterator end() const { return m_coords.end(); }

			bool contains(const value_type &c) const
			{
			  return (std::find(m_coords.begin(), m_coords.end(), c) != m_coords.end());
			}

			void erase(const value_type &c) { m_coords.erase(c); }
			void clear() { m_coords.clear(); }
			bool empty() const { return m_coords.empty(); }
			std::size_t size() const { return m_coords.size(); }
			void append(const value_type &c) { m_coords.insert(c); }

			void append(const area& o)
			{
			  BOOST_FOREACH(const value_type &c, o)
				  this->append(c);
			}

			static area create(const coord &middle, int range)
			{
				area a;

				const int x = middle.x;
				const int y = middle.y;

				int temp_range = range;

				for(int i = 0; i < range+1; i++)
				{
					for(int j = 0; j < temp_range+1; j++)
					{
					  a.append(coord(x + i, y + j));
					  a.append(coord(x - i, y + j));
					  a.append(coord(x + i, y - j));
					  a.append(coord(x - i, y - j));
					}
					temp_range--;
				}

				return a;
			}

		private:
			container_t m_coords;
	};
}

#endif // AREA_H

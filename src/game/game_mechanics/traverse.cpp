#include "traverse.h"
#include "game/coord.h"

#include "move_utilities.h"

namespace aw
{
	namespace game_mechanics
	{
		void traverse::calculate(const map::ptr &map, const coord &origin)
		{
			this->calculate(map, origin, map->get_unit(origin.x, origin.y));
		}

		void traverse::calculate(const map::ptr &map, const coord &origin, const unit::ptr &u, int range)
		{
			m_map = map;

			if(m_coords.empty() || m_start != origin)
			{
				m_start = origin;

				m_coords.clear();

				const int x = origin.x;
				const int y = origin.y;

				const int mp = (range != -1) ? range : u->movement_range();
				const int gas = u->fuel();
//				terrain::types  tind = m_map->get_terrain(x, y)->type();

				//d = direction, 0=north, 1=east, 2=south, 3=west, this sends probes in all 4 directions
				move(x, y+1, 0, mp, gas, false, false, u);
				move(x+1, y, 1, mp, gas, false, false, u);
				move(x, y-1, 2, mp, gas, false, false, u);
				move(x-1, y, 3, mp, gas, false, false, u);
			}
		}

		//verschachtelte Templates und typedefs machen probleme
		const traverse::area_type &traverse::get_coordinates()
		{
			return m_coords;
		}

		int traverse::get_rest_mp(int x, int y) const
		{
			std::map<coord, int>::const_iterator it = m_move_range_left.find(coord(x, y));
			if(it != m_move_range_left.end())
				return it->second;
			else
				return -1;
		}

		void traverse::print(std::ostream &o) const
		{
			for(int y = 0; y < 20; y++)
			{
				for(int x = 0; x < 30; x++)
				{
					if(m_coords.contains(coord(x, y)))
						o << "1 ";
					else
						o << "0 ";
				}
				o << '\n';
			}
		}

//		void traverse::move(int x, int y, int dir, int rest_movement_range, int rest_gas, bool left, bool right, unit::ptr u, terrain::types previous_terrain)
		void traverse::move(int x, int y, int dir, int rest_movement_range, int rest_gas, bool left, bool right, const unit::ptr &u)
		{
			if(m_map->on_map(x, y))
			{
				const int move_costs = m_map->get_terrain(x, y)->movement_costs(u->get_move_type());

				if(move_costs != -1)
				{
				    rest_movement_range -= move_costs;
				    rest_gas-= move_costs;
				}
				else
				{
					//impassible
					return;
				}

				if(rest_gas < 0 || rest_movement_range < 0)
					return;

//				terrain::types current_terrain = m_map->get_terrain(x, y)->type();

				if(can_pass(m_map, coord(x, y), u))
				{
					m_coords.push_back(coord(x, y));

					std::map<coord, int>::iterator it = m_move_range_left.find(coord(x, y));

					//Wenn noch nicht gecheckt, oder weg mit weniger verbrauch gefunden.
					if(it == m_move_range_left.end() || it->second <= rest_movement_range)
					{
						m_move_range_left[coord(x, y)] = rest_movement_range; // Hält immer den kleinstmöglichen mp-Vebrauch

						for(int i= -1; i < 2; i++)
						{
							//Verstehe ich selber nicht so ganz :D
							if(!((left == true && i == -1) || (right == true && i == 1)))
							{
								switch((dir+i)%4)
								{
									case -1:
									case 3:
										//0=north, 1=east, 2=south, 3=west
										move(x-1, y, 3, rest_movement_range, rest_gas, i==-1, i==1, u);
										break;
									case 0:
										move(x, y+1, 0, rest_movement_range, rest_gas, i==-1, i==1, u);
										break;
									case 1:
										move(x+1, y, 1, rest_movement_range, rest_gas, i==-1, i==1, u);
										break;
									case 2:
										move(x, y-1, 2, rest_movement_range, rest_gas, i==-1, i==1, u);
										break;
								}
							}
						}
					}
				} // bool can:overdrive()
			} // bool on_map()
		}

		std::ostream &operator<<(std::ostream &o, const traverse &t)
		{
			t.print(o);
			return o;
		}
	}
}

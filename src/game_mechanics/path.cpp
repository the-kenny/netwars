#include "path.h"
#include "move_utilities.h"

namespace aw
{
	namespace game_mechanics
	{
		path::path(const coord &start, const coord &end)
		: m_recalculate(true), m_start(start), m_end(end), m_fuel_costs(0)
		{
		}

		void path::remove_element(const coord &c)
		{
			m_path.erase(c);
			m_fuel_costs -= m_map->get_terrain(c.x, c.y)->movement_costs(m_map->get_unit(m_start.x, m_start.y)->get_move_type());
		}

		void path::reset()
		{
			m_path.clear();
			m_fuel_costs = 0;
			m_recalculate = true;
		}

		void path::print(std::ostream &o) const
		{
			for(int y = 0; y < 20; y++)
			{
				for(int x = 0; x < 30; x++)
				{
					if(m_path.contains(coord(x, y)))
						o << '1';
					else
						o << '0';
				}
				o << '\n';
			}
		}

//		void path::set_end(const coord &end)
//		{
//			m_end = end;
//
//			if(!m_path.contains(end))
//				m_path.clear();
//
//			m_recalculate = true;
//		}



		void path::calculate(const map::ptr &map, const traverse::ptr &t, const coord &start, const coord &end)
		{
			const unit::ptr &u = map->get_unit(start.x, start.y);
			this->calculate(map, t, start, end, u);
		}

		void path::calculate(const map::ptr &map, const traverse::ptr &t, const coord &start, const coord &end, const unit::ptr &unit, int fuel)
		{
			if(start == end)
			{
				return;
			}

			assert(start);
			assert(end);
			assert(start != end);

			assert(t->get_origin() == start);

			//NOTE: Optimizations should start here (partially recalculation)

			//the start is the same

			if(m_start == start && m_end != end && m_path.contains(end))
			{
				path_t p(m_path);
				int costs = m_fuel_costs;

				BOOST_FOREACH(const coord &c, m_path)
				{
					if(m_traverse->get_rest_mp(m_end.x, m_end.y) >= m_traverse->get_rest_mp(c.x, c.y))
					{
						this->remove_element(c);
					}
				}

				m_end = end;
				m_fuel_costs = costs;
				m_path = p;
			}
			else// if(m_start != start || m_path.empty()) //start has changed - recalculate
			{
				this->reset();

				m_traverse = t;
				m_map = map;

				const int x = start.x;
				const int y = start.y;

				const int to_x = end.x;
				const int to_y = end.y;

				const int mp = unit->movement_range();
				const int gas = (fuel != -1) ? fuel : unit->fuel();
//				terrain::types  tind = m_map->get_terrain(x, y)->type();

				//d = direction, 0=north, 1=east, 2=south, 3=west, this sends probes in all 4 directions
				bool done = false;

				done = move(x, y-1, to_x, to_y, 0, mp, gas, false, false, unit);

				if(!done)
					done = move(x+1, y, to_x, to_y, 1, mp, gas, false, false, unit);
				if(!done)
					done = move(x, y+1, to_x, to_y, 2, mp, gas, false, false, unit);
				if(!done)
					done = move(x-1, y, to_x, to_y, 3, mp, gas, false, false, unit);

				if(!done)
					this->reset();
				else
					m_recalculate = false;

				m_start = start;
				m_end = end;
			}
//			else
//			{
//				traverse ext_t;
//				ext_t.calculate(map, m_end, unit, unit->movement_range());
//
//				path extended_path; //Hold the extended path (w.o. original)
//				extended_path.calculate(map, traverse::ptr(new traverse(ext_t)), m_end, end, unit, unit->fuel() - this->get_fuel_costs());
//
//				path full_extended_path(*this);
//				full_extended_path.append(extended_path);
//
//				path new_path;
//				new_path.calculate(map, t, start, end);
//
//				//int extended_costs = this->get_fuel_costs() + extended_path.get_fuel_costs() - ext_t.get_rest_mp(end.x, end.y);
//				int extended_costs = full_extended_path.get_fuel_costs();
//				int new_costs = new_path.get_fuel_costs();
//
//				std::cout << "==========" << "\n Extended: " << extended_costs << '\n'
//							 << "New Path: " << new_costs << std::endl;
//
//				if(extended_costs <= new_costs)
//					*this = full_extended_path;
//				else
//					*this = new_path;
//			}
		}

//		void path::recalculate(const map::ptr &map, const traverse::ptr &t)
//		{
//			if(m_recalculate)
//				this->calculate(map, t);
//		}

//		bool path::move(int x, int y, int end_x, int end_y, int dir, int rest_movement_range, int rest_gas, bool left, bool right, const unit::ptr &u, terrain::types previous_terrain)
		bool path::move(int x, int y, int end_x, int end_y, int dir, int rest_movement_range, int rest_gas, bool left, bool right, const unit::ptr &u)
		{
			if(m_map->on_map(x, y))
			{
				const int move_costs = m_map->get_terrain(x, y)->movement_costs(u->get_move_type());

				if(move_costs != -1)
				{
					rest_movement_range -= move_costs;
					rest_gas -= move_costs;
				}
				else
				{
					//impassible
					return false;
				}

				if(rest_gas < 0 || rest_movement_range < 0)
					return false;

//				terrain::types current_terrain = m_map->get_terrain(x, y)->type();

				if(can_pass(m_map, coord(x, y), u))
				{
					if(x == end_x && y == end_y)
					{
						if(rest_movement_range == m_traverse->get_rest_mp(x, y))  // Hält immer den kleinstmöglichen mp-Vebrauch
						{
							m_fuel_costs = u->fuel() - rest_gas;
							append(x, y);
							return true;
						}
					}

					//Wenn noch nicht gecheckt, oder weg mit weniger verbrauch gefunden.
					if(m_traverse->get_rest_mp(x, y) == -1 || m_traverse->get_rest_mp(x, y) <= rest_movement_range)
					{
						for(int i= -1; i < 2; i++)
						{
							bool done = false;
							//Verstehe ich selber nicht so ganz :D
							if(!((left == true && i == -1) || (right == true && i == 1)))
							{
								switch((dir+i)%4)
								{
									case -1:
									case 3:
									//0=north, 1=east, 2=south, 3=west
										done = move(x-1, y, end_x, end_y, 3, rest_movement_range, rest_gas, i==-1, i==1, u/*, current_terrain*/);
										break;
									case 0:
										done = move(x, y-1, end_x, end_y, 0, rest_movement_range, rest_gas, i==-1, i==1, u/*, current_terrain*/);
										break;
									case 1:
										done = move(x+1, y, end_x, end_y, 1, rest_movement_range, rest_gas, i==-1, i==1, u/*, current_terrain*/);
										break;
									case 2:
										done = move(x, y+1, end_x, end_y, 2, rest_movement_range, rest_gas, i==-1, i==1, u/*, current_terrain*/);
										break;
								}

								if(done)
								{
									append(x, y);
									return true;
								}
							}
						}
					}
				} // bool can:overdrive()
			} // bool on_map()

			return false;
		}

		void path::append(int x, int y)
		{
			m_path.push_back(coord(x, y));
		}

		void path::append(const path &o)
		{
			m_path += o.m_path;
			m_fuel_costs += o.m_fuel_costs;
			this->remove_doubles();
			this->recalculate_costs(m_map->get_unit(m_start.x, m_start.y));
		}

		void path::recalculate_costs(const unit::ptr &u)
		{
			m_fuel_costs = 0;
			BOOST_FOREACH(const coord &c, m_path)
			{
				m_fuel_costs += m_map->get_terrain(c.x, c.y)->movement_costs(u->get_move_type());
			}
		}

		void path::remove_doubles()
		{
			path_t found;

			BOOST_FOREACH(const coord &c, m_path)
			{
				if(!found.contains(c))
					found.insert(c);
				else
					std::clog << "Double-Item found: " << c << std::endl;
			}

			m_path = found;
		}
	}
}

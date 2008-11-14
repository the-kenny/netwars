#include "explosion_damage.h"

using namespace aw::game_mechanics;

float explosion_damage::get_hp(const coord &c)
{
	iterator it = m_calculated_hp.find(c);

	if(it == m_calculated_hp.end())
		return -1;
	else
		return it->second;
}

void explosion_damage::calculate(const map::ptr &map, const coord &c, int range, int damage)
{
	m_area = aw::area::create(c, range);

	BOOST_FOREACH(const coord &c, m_area)
	{
		unit::ptr u;
		if(map->on_map(c.x, c.y) && (u = map->get_unit(c.x, c.y)) != NULL && !u->is_dummy())
		{
			float life = u->get_hp_as_float();
			if(life-damage < 1)
				life = 1;
			else
				life -= damage;

			m_calculated_hp.insert(std::make_pair(c, life));
		}
	}
}



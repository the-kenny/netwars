#ifndef AW_GAME_INTERNALS_PRICES_H
	#define AW_GAME_INTERNALS_PRICES_H

#include "../units/unit_loader.h"

namespace aw
{
	namespace game_mechanics
	{
		template<typename unit_t>
		int price(typename unit_t::types t)
		{
			int ret = -1;

			ret = aw::unit_list().get_unit_info(t).price;

			return ret;
		}
	}
}

#endif

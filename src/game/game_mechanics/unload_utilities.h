#ifndef AW_UNLOAD_UTILS_H
	#define AW_UNLOAD_UTILS_H

#include "game/area.h"
#include "game/map.h"
#include "game/coord.h"

namespace aw
{
	namespace game_mechanics
	{
		bool can_unload(const map::ptr &map, const coord &transporter_c);
		bool can_unload(const map::ptr &map, const coord &transporter_c, int index);
		area get_unload_coordinates(const map::ptr &map, const coord &transporter_c, int index);
		const std::vector<std::size_t> get_unloadable_indices(const map::ptr &map, const coord &transporter_c);
		const std::vector<std::pair<std::size_t, unit::ptr> > get_unloadable_units(const map::ptr &map, const coord &transporter_c);
	}
}

#endif

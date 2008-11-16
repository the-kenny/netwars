#ifndef AW_GRAPHICS_PATHS_H
	#define AW_GRAPHICS_PATHS_H

#include "game/terrain.h"
#include "game/units/unit_base.h"

namespace aw
{
	namespace gui
	{
		std::string get_path(terrain::types type, terrain::directions direction, terrain::extras extra = terrain::NONE);
		std::string get_path(unit::types type, unit::colors color);
		std::string get_path(unit::extras extra, int live = 0);
	}
}

	#endif

#ifndef AW_DRAWING_H
	#define AW_DRAWING_H

#include <string>

#include "game/scene.h"
#include "game/coord.h"

namespace aw 
{
	namespace gui
	{
		std::string get_sprite_for(const coord& c, const scene::ptr& scene);
		bool is_connectable(terrain::types lhs, terrain::types rhs);
	}
}

#endif

#include "map_widget.h"

#include "game/units/units.h"

#include "game/config.h"
#include "gui/sprites.h"
#include "gui/paths.h"

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

namespace aw
{
	namespace gui
	{
		map_widget::map_widget()
		{
		}


		void map_widget::display(const scene::ptr &p)
		{
			m_scene = p;
			this->queue_draw();

//			std::clog << "Szene gewechselt" << std::endl;
		}


		void map_widget::reset()
		{
			m_scene.reset();
			this->queue_draw();
		}
	}
}

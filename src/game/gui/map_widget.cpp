#include "map_widget.h"

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include "game/scene.h"

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

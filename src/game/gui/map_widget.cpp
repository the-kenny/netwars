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

		/*
		bool is_connectable(terrain::types lhs, terrain::types rhs)
		{
			switch(lhs)
			{
				case terrain::STREET:
					if(rhs == terrain::STREET || rhs == terrain::BRIDGE)
					return true;
				break;
				case terrain::PIPE:
				case terrain::SEGMENT_PIPE:
					if(rhs == terrain::PIPE || rhs == terrain::SEGMENT_PIPE || rhs == terrain::WRECKAGE || rhs == terrain::BASE)
					return true;
				break;
				case terrain::WRECKAGE:
					if(rhs == terrain::PIPE || rhs == terrain::SEGMENT_PIPE)
					return true;
				break;
				case terrain::BRIDGE:
					if(rhs == terrain::BRIDGE || (terrain::is_ground(rhs) && rhs != terrain::RIVER))
					return true;
				break;
				case terrain::RIVER:
					if(rhs == terrain::RIVER || rhs == terrain::BRIDGE)
					return true;
				break;

				default:
					break;
			}

			return false;
		}
		*/
	}
}

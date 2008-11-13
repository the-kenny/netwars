#ifndef AW_GUI_UNIT_UNLOAD_MENU_H
	#define AW_GUI_UNIT_UNLOAD_MENU_H

#include "../units/unit_base.h"

#include <gtkmm/window.h>

namespace aw
{
	namespace gui
	{
		class unit_unload_menu
		{
			public:
				virtual int run() = 0;

				virtual void set_units(const std::vector<unit::ptr> &vec) = 0;
		};
	}
}

#endif

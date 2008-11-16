#ifndef AW_GUI_UNIT_UNLOAD_MENU_H
	#define AW_GUI_UNIT_UNLOAD_MENU_H

#include "game/units/unit_base.h"

namespace aw
{
	namespace gui
	{
		class unit_unload_menu
		{
			public:				
				//Return type: the position of the selected unit in the list of loaded units
				virtual int run() = 0; 

				virtual void set_units(const std::list<unit::ptr> &vec) = 0;
		};
	}
}

#endif

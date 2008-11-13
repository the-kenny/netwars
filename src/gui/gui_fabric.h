#ifndef AW_GUI_FABRIC_H
	#define AW_GUI_FABRIC_H

#include "buy_menu.h"
#include "game_dialog.h"
#include "main_window.h"
#include "unit_action_menu.h"
#include "unit_unload_menu.h"
#include "unit_widget.h"

namespace aw
{
	//TODO: Make a function to show a dialog
	namespace gui
	{
		enum gui_types { GUI_GTK };

		inline gui_types get_gui_type()
		{
			return GUI_GTK;
		}

		buy_menu *create_buy_menu();
		game_dialog *create_game_dialog();
		main_window *create_main_window();
		unit_action_menu *create_unit_action_menu();
		unit_unload_menu *create_unload_menu();
		unit_widget *create_unit_widget();
	}
}

#endif

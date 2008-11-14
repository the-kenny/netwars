#include "gui_fabric.h"

#include "gtk/buy_menu.h"
#include "gtk/game_dialog.h"
#include "gtk/main_window.h"
#include "gtk/unit_action_menu.h"
#include "gtk/unit_unload_menu.h"
#include "gtk/unit_widget.h"

namespace aw
{
	namespace gui
	{
		aw::gui::buy_menu *create_buy_menu()
		{
			aw::gui::buy_menu *ret = NULL;

			switch(aw::gui::get_gui_type())
			{
				case aw::gui::GUI_GTK:
					ret = new aw::gui::gtk::buy_menu;
					break;
			}

			return ret;
		}

		aw::gui::game_dialog *create_game_dialog()
		{
			aw::gui::game_dialog *ret = NULL;

			switch(aw::gui::get_gui_type())
			{
				case aw::gui::GUI_GTK:
					ret = new aw::gui::gtk::new_game_dialog;
					break;
			}

			return ret;
		}

		aw::gui::main_window *create_main_window()
		{
			aw::gui::main_window *ret = NULL;

			switch(aw::gui::get_gui_type())
			{
				case aw::gui::GUI_GTK:
					ret = new aw::gui::gtk::main_window;
					break;
			}

			return ret;
		}

		aw::gui::unit_action_menu *create_unit_action_menu()
		{
			aw::gui::unit_action_menu *ret = NULL;

			switch(aw::gui::get_gui_type())
			{
				case aw::gui::GUI_GTK:
					ret = new aw::gui::gtk::unit_action_menu;
					break;
			}

			return ret;
		}

		aw::gui::unit_unload_menu *create_unload_menu()
		{
			aw::gui::unit_unload_menu *ret = NULL;

			switch(aw::gui::get_gui_type())
			{
				case aw::gui::GUI_GTK:
					ret = new aw::gui::gtk::unit_unload_menu;
					break;
			}

			return ret;
		}

		aw::gui::unit_widget *create_unit_widget()
		{
//			switch(aw::gui::get_gui_type())
//			{
//				case aw::gui::GUI_GTK:
//				{
					return Gtk::manage(new aw::gui::gtk::unit_widget);
//					break;
//				}
//			}
		}
	}
}

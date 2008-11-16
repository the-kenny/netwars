#include "gui_fabric.h"

#ifdef GUI_GTK
#include "gtk/buy_menu.h"
#include "gtk/game_dialog.h"
#include "gtk/main_window.h"
#include "gtk/unit_action_menu.h"
#include "gtk/unit_unload_menu.h"
#include "gtk/unit_widget.h"
#endif

namespace aw
{
	namespace gui
	{
		aw::gui::buy_menu *create_buy_menu(unit::workshops shop, const player::ptr &player)
		{
			#ifdef GUI_GTK
				return new aw::gui::gtk::buy_menu(shop, player);
			#endif
		}
		
		aw::gui::game_dialog *create_game_dialog()
		{
			#ifdef GUI_GTK
				return new aw::gui::gtk::new_game_dialog;
			#endif
		}

		aw::gui::main_window *create_main_window()
		{
			#ifdef GUI_GTK
				return new aw::gui::gtk::main_window;
			#endif
		}

		aw::gui::unit_action_menu *create_unit_action_menu(const std::list<units::actions>& actions)
		{
			#ifdef GUI_GTK
				return new aw::gui::gtk::unit_action_menu(actions);
			#endif
		}

		aw::gui::unit_unload_menu *create_unload_menu(const std::list<unit::ptr>& units)
		{
			#ifdef GUI_GTK
				return new aw::gui::gtk::unit_unload_menu(units);
			#endif
		}

		aw::gui::unit_widget *create_unit_widget()
		{
			#ifdef GUI_GTK
				return Gtk::manage(new aw::gui::gtk::unit_widget);
			#endif
		}
		
	}
}

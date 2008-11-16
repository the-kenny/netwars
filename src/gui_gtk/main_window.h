#ifndef AW_GTK_MAIN_WINDOW_H
	#define AW_GTK_MAIN_WINDOW_H

#include <gtkmm.h>


#include "game/gui/map_widget.h"
#include "game/gui/main_window.h"

#include "game/game_controller.h"
#include "game/units/unit_base.h"
#include "game/terrain.h"


namespace aw
{
	namespace gui
	{
		namespace gtk
		{
			class main_window: public Gtk::Window, public aw::gui::main_window
			{
				public:
					main_window();

					virtual void start_new_game(const aw::game::ptr &ptr);
					virtual void quit_game();
					
					virtual void lock_game() { m_map_widget->disable(); }
					virtual void unlock_game() { m_map_widget->enable(); }

				private:
					void generate_menu();

					Glib::RefPtr<Gtk::UIManager> m_menu_manager;
			};
		}
	}
}

#endif

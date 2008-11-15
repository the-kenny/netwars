#ifndef AW_GTK_MAIN_WINDOW_H
	#define AW_GTK_MAIN_WINDOW_H

#include <gtkmm/window.h>

#include "gui/display.h"
#include "gui/main_window.h"

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
					
					virtual void lock() { this->set_sensitive(false); }
					virtual void unlock() { this->set_sensitive(true); }

				private:
					void generate_menu();

					Glib::RefPtr<Gtk::UIManager> m_menu_manager;
			};
		}
	}
}

#endif

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

					virtual void init_game();
					virtual void start_new_game();
					virtual void quit_game();

				protected:
//					virtual void on_end_turn();
//					virtual void on_player_defeat(player::ptr looser, player::ptr attacker, game_mechanics::defeat_type type);
//					virtual void on_game_finish(player::ptr winner);


				private:
					void generate_menu();

					Glib::RefPtr<Gtk::UIManager> m_menu_manager;
			};
		}
	}
}

#endif

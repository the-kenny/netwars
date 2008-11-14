#ifndef AW_MAIN_WINDOW_H
	#define AW_MAIN_WINDOW_H

#include <gtkmm/window.h>

#include "gui/display.h"

#include "game/game_controller.h"
#include "game/units/unit_base.h"
#include "game/terrain.h"

namespace aw
{
	namespace gui
	{
		class main_window
		{
			public:
				main_window();

				virtual void init_game();
				virtual void start_new_game();
				virtual void quit_game();

			protected:
				virtual void on_end_turn();
				virtual void on_player_defeat(player::ptr looser, player::ptr attacker, game_mechanics::defeat_type type);
				virtual void on_game_finish(player::ptr winner);

				typedef aw::display::map_widget map_widget;
				map_widget::ptr m_map_widget;

				aw::game_controller::ptr m_game_controller;
		};
	}
}

#endif

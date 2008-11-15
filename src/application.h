#ifndef AW_APPLICATION_H
	#define AW_APPLICATION_H
	
#include "gui/main_window.h"	
#include "gui/gtk/main_window.h"

#include "game/units/unit_base.h"
#include "game/player.h"

#include "game/utility.h"

namespace aw
{
	class application: public utility::singleton<application>
	{
		public:
			application();
			int run(int &argc, char** &argv);
			
			void start_new_game();
			
		private:
			// ------------- Menu functions -------------
			unit::types show_buy_menu(unit::workshops shop, const player::ptr &player);
			units::actions show_unit_action_menu(const std::list<units::actions>&);
			int show_unit_unload_menu(const std::list<unit::ptr>&);
			
			//------------- Callbacks -------------
			void end_turn();
			
			
			
			void on_end_turn();
			void on_player_defeat(player::ptr looser, player::ptr attacker, game_mechanics::defeat_type type);
			void on_game_finish(player::ptr winner);


			gui::main_window *m_main_window;
			aw::game_controller::ptr m_game_controller;
	};
}
	
#endif

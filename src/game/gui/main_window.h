#ifndef AW_MAIN_WINDOW_H
	#define AW_MAIN_WINDOW_H

#include "map_widget.h"

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
				typedef boost::signal<void ()> general_signal_t;
				
				virtual void start_new_game(const aw::game::ptr &game);
				virtual void quit_game();
				
				virtual void connect_game_controller(const game_controller::ptr &ptr);
				
				virtual void lock_game() = 0;
				virtual void unlock_game() = 0;
				
				general_signal_t& signal_new_game() { return m_signal_new_game; }
				general_signal_t& signal_end_turn() { return m_signal_end_turn; }
				
			protected:
				typedef aw::gui::map_widget map_widget;
				map_widget::ptr m_map_widget;
				
				general_signal_t m_signal_new_game;
				general_signal_t m_signal_end_turn;
				
				void trigger_new_game() { m_signal_new_game(); } //UGLY
				void trigger_end_turn()  { m_signal_end_turn(); }
		};
	}
}

#endif

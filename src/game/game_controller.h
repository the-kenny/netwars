#ifndef AW_GAME_CONTROLLER_H
#define AW_GAME_CONTROLLER_H

#include "game.h"

#include "game_mechanics/traverse.h"
#include "game_mechanics/path.h"
#include "scene.h"
#include "units/actions.h"

#include <list>

#include <boost/signals.hpp>
#include <boost/utility.hpp>

/*
* Moving
  After a click on a unit is game::begin_unit_move() executed.
  On mouse-movements will the path recalculated.
  If the movement is canceled, game::cancel_unit_move() will executed.

* Actions
  The possible actions are calculated in game_controller and are 
  choosed trough a callback.

* Attack
  After a unit was selected to attack, the possible targets will 
  calculated.
  After the target was selected, complete_unit_move() and
  game::attack_unit() are executed (In this order!)

* Explosions
  m_gamestate is set to ATTACKING
  The range is shown and after the confirmation is complete_unit_move()
  and then explode() executed.

* Takeovers
  complete_unit_move() and capture_building() is executed.

* Loading
  game::load_unit() is executed 
  (complete_unit_move() will called from aw::game)

* Unloading
  The loaded units are chosen with a callback.
  If the unloading is canceled, cancel_unit_move() is executed.
  game::unload_unit(from, to) and executed, and after that,
  complete_unit_move() is executed.

* Hidi/Appear:
  game::[hide|appear]_unit() is executed.

* Silos
  m_gamestate is set to ATTACKING. After the target is choosen,
  game::launch_silo() is executed. 
*/

namespace aw
{
	class game_controller
	{
		public:
			typedef boost::shared_ptr<game_controller> ptr;
			
			typedef boost::signal<void (const scene::ptr&)> scene_changed_t;
			
			//Menu-Callbacks
			typedef boost::signal<units::actions (const std::list<units::actions>&)> unit_action_menu_callback_t;
			typedef boost::signal<unit::types (unit::workshops, const player::ptr&)> unit_buy_menu_callback_t;
			typedef boost::signal<int (const std::list<unit::ptr> &)> unit_unload_menu_callback_t;

			//Misc callbacks
			typedef boost::signal<void (const unit::ptr&)> unit_clicked_callback_t;
			typedef boost::signal<void (const terrain::ptr&)> terrain_clicked_callback_t;

			enum gamestate { IDLE, DISPLAYING, MOVING, ATTACKING, UNLOADING, REPAIRING };

			game_controller();
			void start_game(const game::ptr &game);
			void start_turn();
			void end_turn();

			// 1 = left click, 3 = right click
			void click(const coord &pos, int key);
			void mouse_hover_changed(const coord &pos);

			scene_changed_t &signal_scene_change() { return m_signal_scene_changed; }
			
			//Callback access methods
			unit_action_menu_callback_t &signal_show_unit_action_menu() { return m_unit_action_menu_callback; }
			unit_buy_menu_callback_t &signal_show_buy_menu() { return m_unit_buy_menu_callback; }
			unit_unload_menu_callback_t &signal_show_unload_menu() { return m_unit_unload_menu_callback; }

			//General callback accessor methods
			unit_clicked_callback_t &signal_unit_clicked() { return m_signal_unit_clicked; }
			terrain_clicked_callback_t &signal_terrain_clicked() { return m_signal_terrain_clicked; }
			
		private:
			void on_unit_click(const coord &pos, int key);
			void on_building_click(const coord &pos, int key);
			void on_free_click(const coord &pos, int key);

			units::actions show_actions(const coord &pos);
			void process_action(units::actions action, const coord &pos);

			scene_changed_t m_signal_scene_changed;
			void update_display();

			gamestate m_gamestate;
			coord m_selection;
			std::size_t m_unload_index;

			game_mechanics::traverse::ptr m_traverse;
			game_mechanics::path::ptr m_path;
			area m_highlighted_area;

			game::ptr m_game;
			
			unit_action_menu_callback_t m_unit_action_menu_callback;
			unit_buy_menu_callback_t m_unit_buy_menu_callback;
			unit_unload_menu_callback_t m_unit_unload_menu_callback;

			//General callbacks
			unit_clicked_callback_t m_signal_unit_clicked;
			terrain_clicked_callback_t m_signal_terrain_clicked;
	};
}

#endif // AW_GAME_CONTROLLER_H

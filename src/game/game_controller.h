#ifndef AW_GAME_CONTROLLER_H
#define AW_GAME_CONTROLLER_H

#include "game.h"

#include "game_mechanics/traverse.h"
#include "game_mechanics/path.h"
#include "scene.h"
#include "units/actions.h"

#include <list>

#include <boost/signals2/signal.hpp>
#include <boost/utility.hpp>

/*
  Bewegen
  Nach einem Klick auf eine Einheit wird game::begin_unit_move() ausgeführt.
  Bei Mausbewegung wird der Path neu berechnet.
  Abbruch
  Wird die Bewegung abgebrochen, wird game::cancel_unit_move() ausgeführt.

  Aktionen
  Mögliche Aktionen werden in game_controller errechnet und angezeigt

  Angriff
  Nachdem Angriff ausgewählt wurde, werden die Möglichkeiten errechnet.
  Nach der bestätigung wird complete_unit_move()  und game::attack_unit() ausgeführt.

  Explodieren
  gamestate wird zu attacking.
  Explosionsreichweite wird angezeigt und nach Konfirmation wird complete_unit_move() und explode() aufgerufen.

  Übernehmen
  complete_unit_move() und capture_building() werden aufgerufen

  Einladen
  game::load_unit() wird aufgerufen (ruft complete_unit_move() auf)

  Ausladen:
  Beim ausladen wird erst das Menü angezeigt, Nach der Auswahl einer Einheit werden die möglichen Ausladefelder angezeigt.
  Wird vor dem ersten Ausladen abgebrochen, wird cancel_unit_move() aufgerufen.
  Beim ausladen wird game::unload_unit(from, to) aufgerufen (berechnet Sprit usw.)
  Nachdem die erste Einheit ausgeladen wurde, wird complete_unit_move() ausgeführt.

  Hide
  game::hide_unit() wird aufgerufen

  Appear
  game::appear_unit() wird aufgerufen

  Cancel
  game::cancel_unit_move() wird aufgerufen

  Raketensilo
  gamestate wird zu attacking und es wird der Zielort ausgewählt. Danach wird game::launch_silo() ausgeführt
  Bei abbruch wird das auswhählen des Zielpunktes abgebrochen und das Menü gezeigt.
  Eventuell wird die Einheit auch nur zurück ins Bewegen versetzt, je nach komplexität


  Anzeige
  Die Szene wird von der map angenommen, wird von game eventuell verändert, und schließlich in game_controller vervollständigt.

  Vielleicht sollte die komplette Szene erst in game_controller erzeugt werden.
  Map erzeugt und game verarbeitet dann nur ein Zwischenobjekt, was nur die Map beinhaltet,

  Vielleicht sollte scene umgebaut werden, so dass sie eine liste mit pair<string, area> enthält.
*/

/*
//NOTE: Ideas for signals:

signal_unit_clicked()
signal_terrain_clicked()
*/

namespace aw
{
  //NOTE: Eventuell einen neuen State "SELECTING" einführen und die im Menü ausgewählte Aktion in einer Variable speichern
  class game_controller
  {
  public:
	typedef boost::shared_ptr<game_controller> ptr;
			
	typedef boost::signals2::signal<void (const scene::ptr&)> scene_changed_t;
			
	//Menu-Callbacks
	typedef boost::signals2::signal<units::actions 
									(const std::list<units::actions>&)> unit_action_menu_callback_t;
	typedef boost::signals2::signal<unit::types (unit::workshops, const player::ptr&)> unit_buy_menu_callback_t;
	typedef boost::signals2::signal<int (const std::list<unit::ptr> &)> unit_unload_menu_callback_t;

	//Misc callbacks
	typedef boost::signals2::signal<void 
									(const unit::ptr&)> unit_clicked_callback_t;
	typedef boost::signals2::signal<void 
									(const terrain::ptr&)> terrain_clicked_callback_t;

	enum gamestate { IDLE, DISPLAYING, 
					 MOVING, ATTACKING, 
					 UNLOADING, REPAIRING };

	game_controller();
	void start_game(const game::ptr &game);
	void start_turn();
	void end_turn();

	// 1 = left click, 3 = right click
	void click(const coord &pos, int key);
	void mouse_hover_changed(const coord &pos);

	scene_changed_t &signal_scene_change() { return m_signal_scene_changed; }
			
	//Callback access methods
	unit_action_menu_callback_t &signal_show_unit_action_menu() { 
	  return m_unit_action_menu_callback; 
	}
	unit_buy_menu_callback_t &signal_show_buy_menu() { 
	  return m_unit_buy_menu_callback; 
	}
	unit_unload_menu_callback_t &signal_show_unload_menu() { 
	  return m_unit_unload_menu_callback; 
	}

	//General callback accessor methods
	unit_clicked_callback_t &signal_unit_clicked() { 
	  return m_signal_unit_clicked; 
	}
	terrain_clicked_callback_t &signal_terrain_clicked() { 
	  return m_signal_terrain_clicked; 
	}

	game::ptr game() { 
	  return m_game;
	}

  protected:
	virtual void on_unit_click(const coord &pos, int key);
	virtual void on_building_click(const coord &pos, int key);
	virtual void on_free_click(const coord &pos, int key);

	scene_changed_t m_signal_scene_changed;
	void update_display();

	coord m_selection;
	area m_highlighted_area;
	
	game::ptr m_game;

			
  private:
	units::actions show_actions(const coord &pos);
	void process_action(units::actions action, const coord &pos);

	
	gamestate m_gamestate;
	
	std::size_t m_unload_index;

	game_mechanics::traverse::ptr m_traverse;
	game_mechanics::path::ptr m_path;
			
	unit_action_menu_callback_t m_unit_action_menu_callback;
	unit_buy_menu_callback_t m_unit_buy_menu_callback;
	unit_unload_menu_callback_t m_unit_unload_menu_callback;

	//General callbacks
	unit_clicked_callback_t m_signal_unit_clicked;
	terrain_clicked_callback_t m_signal_terrain_clicked;
  };
}

#endif // AW_GAME_CONTROLLER_H

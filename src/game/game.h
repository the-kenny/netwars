#ifndef AW_GAME_H
#define AW_GAME_H

#include "map.h"
#include "player.h"
#include "game_mechanics/path.h"
#include "game_mechanics/player_utilities.h"
#include "coord.h"

#include <boost/signals2/signal.hpp>

/*
//NOTE: Ideas for signals:

signal_unit_defeated(defeated, attacker)
signal_unit_moved(from, to, unit)
*/

namespace aw
{
  class game
  {
  public:
	typedef boost::shared_ptr<game> ptr;

	typedef boost::signals2::signal<void (player::ptr, 
										  player::ptr, 
										  game_mechanics::defeat_type)> signal_player_defeated_t;
	typedef boost::signals2::signal<void (player::ptr)> signal_game_finished_t;

	//to could be used for the preview of the move
	void begin_unit_move(const coord &from, const coord &to);
	void complete_unit_move(const game_mechanics::path &path);
	void cancel_unit_move();
	void complete_dead_unit_move();

	void attack_unit(const coord &attacker_c, const coord &victim_c);
	void explode(const coord &unit_c);
	void knock_down(const coord &att_c, const coord &target_c);
	void launch_silo(const coord &silo_c, const coord &target_c);
	void capture_building(const coord &unit_c);

	void supply_units(const coord &support_unit);
	void repair_unit(const coord &repair_unit, const coord &to);

	void load_unit(const coord &transporter_c, const coord &load_c, const game_mechanics::path &path); //calls complete_unit_move()
	void unload_unit(const coord &transporter_c, const game_mechanics::path &path, const coord &to, std::size_t index); //takes care about the movement

	void hide_unit(const coord &unit_c);
	void appear_unit(const coord &unit_c);

	void buy_unit(const coord &pos, unit::types type);

	void join_units(const coord &from, const coord &to, const game_mechanics::path &path); //calls complete_unit_move()
	inline unit::ptr get_unit(const coord &c) { return m_map->get_unit(c); }
	inline terrain::ptr get_terrain(const coord &c) { return m_map->get_terrain(c); }

	inline map::ptr get_map() const { return m_map; }
	inline bool on_map(const coord &pos) const { return m_map->on_map(pos); }

	inline player::ptr get_active_player() { return m_players.get_active_player(); }

	//If a move is active, the unit which should be moved will be moved for a preview
	//			display::scene::ptr get_prepared_scene();

	void load_map(const std::string &file);
	void set_map(const map::ptr &map) { m_map = map; }

	void set_funds_per_building(unsigned int funds) { m_fpb = funds; }
	void set_initial_funds(unsigned int funds) { m_initial_funds = funds; }

	void start_game();
	void start_turn();
	void end_turn();

	bool move_active() const { return m_active_move.active; }

	inline signal_player_defeated_t &signal_player_defeated() { return m_signal_player_defeated; }
	inline signal_game_finished_t &signal_game_finished() { return m_signal_game_finished; }

	void remove_player(const player::ptr &player);
	player_manager &get_player_manager() { return m_players; }

  protected:
	void supply_unit_from_building(const coord &support_building);
	void check_for_defeat();
	void check_for_game_end();

  private:
	struct unit_move
	{
	  unit_move()
		: active(false), done(false),  from(coord()), to(coord())
	  {
	  }

	  void reset()
	  {
		active = false;
		done = true;

		from.reset();
		to.reset();
	  }

	  bool active;
	  bool done;
	  coord from;
	  coord to;
	} m_active_move;

	signal_player_defeated_t m_signal_player_defeated;
	signal_game_finished_t m_signal_game_finished;

	map::ptr m_map;
	player_manager m_players;

	unsigned int m_fpb;
	unsigned int m_initial_funds;

	std::map<coord, dummy_unit::ptr> m_removed_dummy_units;
  };
}

#endif

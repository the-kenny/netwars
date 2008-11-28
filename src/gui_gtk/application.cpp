#include "application.h"

#include "game/gui/game_dialog.h"

#include "buy_menu.h"
#include "game_dialog.h"
#include "main_window.h"
#include "unit_action_menu.h"
#include "unit_unload_menu.h"
#include "unit_widget.h"

#include "game/game_controller.h"

#include <gtkmm.h>

#include <boost/bind.hpp>
#include <boost/signals.hpp>


using namespace aw;

application::application()
{
	std::srand(std::time(NULL));
}

int application::run(int &argc, char** &argv)
{
	try
	{
		Gtk::Main kit(argc, argv);

		m_main_window = new gui::gtk::main_window();
		m_main_window->signal_new_game().connect(boost::bind(&application::start_new_game, this));
		m_main_window->signal_end_turn().connect(boost::bind(&application::end_turn, this));
		m_main_window->signal_end_turn().connect(boost::bind(&application::on_end_turn, this));

		Gtk::Main::run(*dynamic_cast<gui::gtk::main_window*>(m_main_window));
	}
	catch(const std::exception &e)
	{
		std::cerr << "Uncaught Exception: " << e.what() << std::endl;
	}

	return 0;
}

// ------------- Menu functions -------------

unit::types application::show_buy_menu(unit::workshops shop, const player::ptr &player)
{
	m_main_window->lock_game();
	gui::buy_menu *menu = new gui::gtk::buy_menu(shop, player);
	unit::types ret = menu->run();
	m_main_window->unlock_game();

	return ret;
}

units::actions application::show_unit_action_menu(const std::list<units::actions>& actions)
{
	m_main_window->lock_game();
	gui::unit_action_menu *menu = new gui::gtk::unit_action_menu(actions);
	units::actions ret = menu->run();
	m_main_window->unlock_game();

	return ret;
}

int application::show_unit_unload_menu(const std::list<unit::ptr>& units)
{
	m_main_window->lock_game();
	gui::unit_unload_menu *menu = new gui::gtk::unit_unload_menu(units);
	int ret = menu->run();
	m_main_window->unlock_game();

	return ret;
}

//------------- Callbacks -------------

void application::end_turn()
{
	m_game_controller->end_turn();
	m_game_controller->start_turn();
}

void application::start_new_game()
{
	gui::game_dialog *g = new gui::gtk::new_game_dialog();
	int response = g->run();

	if(response == gui::OK)
	{
		aw::game::ptr game = aw::game::ptr(new aw::game);

		game->load_map(g->map());

		game->set_funds_per_building(g->funds_per_building());
		game->set_initial_funds(g->initial_funds());

		//Connect the callbacks
		game->signal_player_defeated().connect(boost::bind(&application::on_player_defeat, this, _1, _2, _3));
		game->signal_game_finished().connect(boost::bind(&application::on_game_finish, this, _1));

		m_game_controller = game_controller::ptr(new game_controller);

		// Connect the menus
		m_game_controller->signal_show_buy_menu().connect(boost::bind(&application::show_buy_menu, this, _1, _2));
		m_game_controller->signal_show_unit_action_menu().connect(boost::bind(&application::show_unit_action_menu, this, _1));
		m_game_controller->signal_show_unload_menu().connect(boost::bind(&application::show_unit_unload_menu, this, _1));

		m_main_window->connect_game_controller(m_game_controller);

		m_game_controller->start_game(game);
		m_main_window->start_new_game(game);
	}
}

void application::on_end_turn()
{
	std::cout << "[main_window::on_end_turn] Beende Runde" << std::endl;
}

void application::on_player_defeat(player::ptr looser, player::ptr attacker, game_mechanics::defeat_type type)
{
	std::cout << "The " << looser->get_color_string() << " Player was defeated!" << std::endl;
}

void application::on_game_finish(player::ptr winner)
{
	std::cout << "Game has ended! The " << winner->get_color_string() << " player has won!" << std::endl;
	m_main_window->lock_game();
}


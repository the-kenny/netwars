#include "main_window.h"
#include "game_dialog.h"

#include <boost/bind.hpp>
#include "game/map_loader/map_loader.h"
#include "game/config.h"
#include "game/game.h"
#include "game/coord.h"

#include "gui/gui_fabric.h"

#include <gtkmm.h>

using namespace aw::gui;

main_window::main_window()
{
}

void main_window::init_game()
{
	m_game_controller = game_controller::ptr(new game_controller);

	m_game_controller->signal_scene_change().connect(boost::bind(&map_widget::display, m_map_widget, _1));
	m_map_widget->on_click().connect(boost::bind(&aw::game_controller::click, m_game_controller, _1, _2));
	m_map_widget->on_focus_changed().connect(boost::bind(&aw::game_controller::mouse_hover_changed, m_game_controller, _1));
}

void main_window::quit_game()
{
	m_game_controller.reset();
	m_map_widget->reset();
	m_map_widget->on_click().disconnect_all_slots();
	m_map_widget->on_focus_changed().disconnect_all_slots();
}


void main_window::start_new_game()
{
	m_map_widget->set_sensitive(false);

	gui::game_dialog *g = gui::create_game_dialog();
	int response = g->run();

	if(response == gui::OK)
	{
		//this->quit_game();

		this->init_game();

		aw::game::ptr game = aw::game::ptr(new aw::game);

		game->load_map(g->map());

		game->set_funds_per_building(g->funds_per_building());
		game->set_initial_funds(g->initial_funds());

		game->signal_player_defeated().connect(boost::bind(&main_window::on_player_defeat, this, _1, _2, _3));
		game->signal_game_finished().connect(boost::bind(&main_window::on_game_finish, this, _1));

		m_game_controller->start_game(game);

//		std::string color = m_game->get_active_player()->get_color_string();
//		m_statusbar->push(color + "'s turn.");
	}

	if(m_game_controller)
		m_map_widget->set_sensitive(true);
}

void main_window::on_end_turn()
{
	if(m_game_controller)
	{
		std::cout << "[main_window::on_end_turn] Beende Runde" << std::endl;
		m_game_controller->end_turn();

		m_game_controller->start_turn();
	}
}

void main_window::on_player_defeat(player::ptr looser, player::ptr attacker, game_mechanics::defeat_type type)
{
	std::cout << "The " << looser->get_color_string() << " Player was defeated!" << std::endl;
}

void main_window::on_game_finish(player::ptr winner)
{
	std::ostringstream s;
	s << "Game has ended! The " << winner->get_color_string() << " player has won!" << std::endl;

	m_map_widget->set_sensitive(false);

	this->quit_game();
}




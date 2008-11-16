#include "main_window.h"

#include <boost/bind.hpp>
#include "game/game.h"


using namespace aw;
using namespace aw::gui;


void main_window::quit_game()
{
	//m_game_controller.reset();
	m_map_widget->reset();
	m_map_widget->on_click().disconnect_all_slots();
	m_map_widget->on_focus_changed().disconnect_all_slots();
}

void main_window::start_new_game(const aw::game::ptr &game)
{

}

void main_window::connect_game_controller(const game_controller::ptr &ptr)
{
	ptr->signal_scene_change().connect(boost::bind(&map_widget::display, m_map_widget, _1));	
	
	m_map_widget->on_click().connect(boost::bind(&aw::game_controller::click, ptr, _1, _2));
	m_map_widget->on_focus_changed().connect(boost::bind(&aw::game_controller::mouse_hover_changed, ptr, _1));
}





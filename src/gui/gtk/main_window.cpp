#include "main_window.h"
#include "game_dialog.h"

#include <boost/bind.hpp>
#include "map_loader/map_loader.h"
#include "config.h"
#include "game.h"
#include "coord.h"

#include <gtkmm.h>

using namespace aw::gui::gtk;

main_window::main_window()
{
	m_map_widget = map_widget::ptr(new map_widget);

	Gtk::VBox *v = Gtk::manage(new Gtk::VBox);

	this->generate_menu();

	v->pack_start(*m_menu_manager->get_widget("/menubar"), Gtk::PACK_SHRINK);
	v->pack_start(*m_map_widget);

	m_map_widget->set_sensitive(false);
	this->add(*v);
	this->show_all();
}

void main_window::init_game()
{
	aw::gui::main_window::init_game();
}

void main_window::start_new_game()
{
	aw::gui::main_window::start_new_game();
	m_menu_manager->get_action("/menubar/game_menu/end_turn")->set_sensitive(true);
}

void main_window::quit_game()
{
	aw::gui::main_window::quit_game();
	m_menu_manager->get_action("/menubar/game_menu/end_turn")->set_sensitive(false);
}



void main_window::generate_menu()
{
	Glib::RefPtr<Gtk::ActionGroup> action_group = Gtk::ActionGroup::create();

	action_group->add(Gtk::Action::create("file_menu", "File"));
	action_group->add(Gtk::Action::create("new_game", Gtk::Stock::NEW, "New game"), sigc::mem_fun(*this, &main_window::start_new_game));
	action_group->add(Gtk::Action::create("quit", Gtk::Stock::QUIT, "Quit"), sigc::mem_fun(*this, &main_window::hide));

	action_group->add(Gtk::Action::create("game_menu", "Game"));
	action_group->add(Gtk::Action::create("end_turn", "End Turn"), Gtk::AccelKey("F3"), sigc::mem_fun(*this, &main_window::on_end_turn));

	m_menu_manager = Gtk::UIManager::create();
	m_menu_manager->insert_action_group(action_group);

	this->add_accel_group(m_menu_manager->get_accel_group());

	const Glib::ustring ui =
	"<ui>"
	"	<menubar name='menubar'>"
	"		<menu action='file_menu'>"
	"			<menuitem action='new_game'/>"
	"			<separator />"
	"			<menuitem action='quit'/>"
	"		</menu>"

	"		<menu action='game_menu'>"
	"			<menuitem action='end_turn'/>"
	"		</menu>"
	"	</menubar>"
	"</ui>";

	m_menu_manager->add_ui_from_string(ui);

	m_menu_manager->get_action("/menubar/game_menu/end_turn")->set_sensitive(false);
}

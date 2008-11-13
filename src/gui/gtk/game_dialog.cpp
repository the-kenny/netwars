#include "game_dialog.h"

#include <iostream>
#include <libglademm/xml.h>

using namespace aw::gui::gtk;

 new_game_dialog::new_game_dialog()
 : m_return(false), m_response(aw::gui::CANCEL)
{
	this->set_title("New Game");
	this->set_deletable(false);

	Glib::RefPtr<Gnome::Glade::Xml> xml = Gnome::Glade::Xml::create("data/gui/new_game_window.glade", "root_vbox");

	Gtk::VBox *root_vbox = NULL;
	xml->get_widget("root_vbox", root_vbox);

	if(!root_vbox)
		throw;

	Gtk::Button *start_button = NULL;
	xml->get_widget("start_button", start_button);
	start_button->signal_clicked().connect(sigc::mem_fun(*this, &new_game_dialog::on_start_button_clicked));

	Gtk::Button *cancel_button = NULL;
	xml->get_widget("cancel_button", cancel_button);
	cancel_button->signal_clicked().connect(sigc::mem_fun(*this, &new_game_dialog::on_cancel_button_clicked));

//	Gtk::ToggleButton *player1_toggle = NULL;
//	xml->get_widget("player1_check", player1_toggle);
//	player1_toggle->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &new_game_dialog::player_toggled), player1_toggle, 1));
//	m_players[0] = player1_toggle->get_active();
//
//	Gtk::ToggleButton *player2_toggle = NULL;
//	xml->get_widget("player2_check", player2_toggle);
//	player2_toggle->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &new_game_dialog::player_toggled), player2_toggle, 2));
//	m_players[1] = player2_toggle->get_active();
//
//	Gtk::ToggleButton *player3_toggle = NULL;
//	xml->get_widget("player3_check", player3_toggle);
//	player3_toggle->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &new_game_dialog::player_toggled), player3_toggle, 3));
//	m_players[2] = player3_toggle->get_active();
//
//	Gtk::ToggleButton *player4_toggle = NULL;
//	xml->get_widget("player4_check", player4_toggle);
//	player4_toggle->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &new_game_dialog::player_toggled), player4_toggle, 4));
//	m_players[3] = player4_toggle->get_active();


	Gtk::SpinButton *initial_funds_spin = NULL;
	xml->get_widget("initial_funds_spin", initial_funds_spin);
	initial_funds_spin->signal_value_changed().connect(sigc::bind(sigc::mem_fun(*this, &new_game_dialog::initial_spin_toggled), initial_funds_spin));
	m_initial_funds = initial_funds_spin->get_value_as_int();

	Gtk::SpinButton *fpb_spin = NULL;
	xml->get_widget("fpb_spin", fpb_spin);
	fpb_spin->signal_value_changed().connect(sigc::bind(sigc::mem_fun(*this, &new_game_dialog::fpb_spin_toggled), fpb_spin));
	m_funds_per_building = fpb_spin->get_value_as_int();

	Gtk::Entry *map_entry = NULL;
	xml->get_widget("map_entry", map_entry);

	Gtk::Button *map_select_button = NULL;
	xml->get_widget("map_select_button", map_select_button);
	map_select_button->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &new_game_dialog::on_map_select_button_clicked), map_entry));

//	this->add(*root_vbox);

	this->add(*root_vbox);
}

aw::gui::response_t new_game_dialog::run()
{
	m_return = false;

	this->show_all();

	Glib::RefPtr<Glib::MainContext> c = Glib::MainContext::get_default();

	while(!m_return)
	{
		c->iteration(true);
	}

	this->hide();

	return m_response;
}

//bool new_game_dialog::run()
//{
//	this->add(*root_vbox);
//	this->show_all_children();
//
//
//}

void new_game_dialog::on_start_button_clicked()
{
	if(m_map.empty())
	{
		Gtk::MessageDialog d(*this, "You have to choose a map", Gtk::MESSAGE_WARNING);
		d.run();
		return;
	}


	m_response = aw::gui::OK;
	m_return = true;
}

void new_game_dialog::on_cancel_button_clicked()
{
	m_response = aw::gui::CANCEL;
	m_return = true;
}

//void new_game_dialog::player_toggled(Gtk::ToggleButton *p, int number)
//{
//	m_players[number-1] = p->get_active();
//	std::cout << "Setze player " << number << " auf " << ((m_players[number-1]) ? "true" : "false") << std::endl;
//}


void new_game_dialog::initial_spin_toggled(Gtk::SpinButton *spin)
{
	m_initial_funds = spin->get_value_as_int();
}

void new_game_dialog::fpb_spin_toggled(Gtk::SpinButton *spin)
{
	m_funds_per_building = spin->get_value_as_int();
}

void new_game_dialog::on_map_select_button_clicked(Gtk::Entry *l)
{
	std::string map = this->show_map_selection();
	m_map = map;
	l->set_text(map);
}

std::string new_game_dialog::show_map_selection()
{
	Gtk::FileChooserDialog dialog("Please choose a map", Gtk::FILE_CHOOSER_ACTION_OPEN );
	dialog.set_transient_for(*this);

	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button("Open", Gtk::RESPONSE_OK);

	Gtk::FileFilter aw_files;
	aw_files.set_name("AW-Series Map Editor Files");
	aw_files.add_pattern("*.awm");
	aw_files.add_pattern("*.aw2");
	aw_files.add_pattern("*.awd");
	aw_files.add_pattern("*.aws");
	dialog.add_filter(aw_files);

	Gtk::FileFilter all_files;
	all_files.set_name("All Files");
	all_files.add_pattern("*");
	dialog.add_filter(all_files);

	int result = dialog.run();

	if(result == Gtk::RESPONSE_OK)
		return dialog.get_filename();
	else
		return std::string("");
}





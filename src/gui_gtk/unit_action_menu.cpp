#include "unit_action_menu.h"

#include <gtkmm/button.h>
#include <boost/foreach.hpp>


using namespace aw::gui::gtk;

//Initialize the variables
unit_action_menu::unit_action_menu(const std::list<units::actions>& actions)
: Gtk::Window(Gtk::WINDOW_POPUP), gui::unit_action_menu(actions), m_return(false), m_return_value(units::CANCEL)
{
	m_vbox = Gtk::manage(new Gtk::VBox);
	m_vbox->set_spacing(1);

	Gtk::Button *cancel = Gtk::manage(new Gtk::Button("Cancel"));
	cancel->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &unit_action_menu::on_button_click), units::CANCEL));
	m_vbox->pack_end(*cancel, Gtk::PACK_SHRINK);

	this->add(*m_vbox);

	this->set_modal(true);
	this->set_position(Gtk::WIN_POS_MOUSE);
	this->set_type_hint(Gdk::WINDOW_TYPE_HINT_MENU);
	
	BOOST_FOREACH(units::actions a, actions)
		this->add_action(a);
}

aw::units::actions unit_action_menu::run()
{
	int x = 0;
	int y = 0;
	this->get_position(x, y);

	int width = 0;
	int height = 0;
	this->get_size(width, height);

	this->move(x+(width/2)+1, y+(height/2)+1); //Hack: Mouse have to be moved out of the button

	this->show_all();

	Glib::RefPtr<Glib::MainContext> c = Glib::MainContext::get_default();
	m_return_value = units::CANCEL;

	while(!m_return)
	{
		c->iteration(true);
	}

	this->hide();

	return m_return_value;
}

void unit_action_menu::add_action(units::actions a)
{
	std::string label(aw::gui::unit_action_menu::get_name(a));

	using namespace aw::units;

	//enum actions { ATTACK, WAIT, CAPTURE, LOAD, UNLOAD, HIDE, APPEAR, CANCEL };

	Gtk::Button *b = Gtk::manage(new Gtk::Button(label));
	b->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &unit_action_menu::on_button_click), a));
	m_vbox->pack_start(*b, Gtk::PACK_SHRINK);
}



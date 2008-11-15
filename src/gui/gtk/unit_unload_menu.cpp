#include <gtkmm/box.h>
#include <gtkmm/button.h>

#include <boost/foreach.hpp>

#include "unit_unload_menu.h"
#include "unit_widget.h"
#include "game/units/unit_loader.h"


using namespace aw::gui::gtk;

unit_unload_menu::unit_unload_menu(const std::list<unit::ptr> &units)
: Gtk::Window(Gtk::WINDOW_POPUP), m_return(false), m_return_value(-1)
{
	Gtk::Settings::get_default()->property_gtk_button_images() = true; //To show the units in the buttons

	this->set_modal(true);
	this->set_position(Gtk::WIN_POS_MOUSE);
	this->set_type_hint(Gdk::WINDOW_TYPE_HINT_MENU);
	
	this->set_units(units);
}

int unit_unload_menu::run()
{
	int x = 0;
	int y = 0;
	this->get_position(x, y);

	int width = 0;
	int height = 0;
	this->get_size(width, height);

	this->move(x+(width/2)+1, y+(height/2)+1); //Bugfix: Mouse have to be moved out of the button

	this->show_all();

	Glib::RefPtr<Glib::MainContext> c = Glib::MainContext::get_default();
	m_return_value = -1;

	while(!m_return)
	{
		c->iteration(true);
	}

	this->hide();

	return m_return_value;
}

void unit_unload_menu::on_button_click(int a)
{
	m_return_value = a;
	m_return = true;
}

void unit_unload_menu::set_units(const std::list<unit::ptr> &vec)
{
	Gtk::VBox *v = Gtk::manage(new Gtk::VBox);
	v->set_spacing(1);

	int i = 0;
	BOOST_FOREACH(const unit::ptr &p, vec)
	{
		Gtk::Button *b = Gtk::manage(new Gtk::Button);

		b->set_label(unit_list().get_unit_info(p->type()).name);
		b->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &unit_unload_menu::on_button_click), i));

		aw::gui::gtk::unit_widget *w = Gtk::manage(new gtk::unit_widget);
		w->set_unit(p);
		b->set_image(*w);
		b->set_image_position(Gtk::POS_LEFT);

		v->pack_start(*b);

		i++;
	}

	Gtk::Button *b = Gtk::manage(new Gtk::Button("Cancel"));
	b->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &unit_unload_menu::on_button_click), -1));
	v->pack_start(*b);

	v->children()[0].get_widget()->grab_focus();

	this->add(*v);
}

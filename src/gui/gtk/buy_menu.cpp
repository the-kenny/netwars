#include "buy_menu.h"

#include <vector>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "game/game_mechanics/prices.h"
#include "game/units/unit_loader.h"
#include "game/units/unit_base.h"

#include "gui/sprites.h"
#include "gui/paths.h"

using namespace aw;
using namespace aw::gui::gtk;

namespace
{
	class unit_cellrenderer: public Gtk::CellRenderer
	{
		public:
			unit_cellrenderer()
			: Glib::ObjectBase("unit_cellrenderer"), m_property_type(*this, "type"), m_property_color(*this, "color")
			{
			}

			Glib::PropertyProxy<unit::types> property_type() { return m_property_type.get_proxy(); }
			Glib::PropertyProxy<unit::colors> property_color() { return m_property_color.get_proxy(); }

			virtual void render_vfunc(const Glib::RefPtr<Gdk::Drawable> &window, Gtk::Widget &, const Gdk::Rectangle &, const Gdk::Rectangle &cell_area, const Gdk::Rectangle &/*expose_area*/, Gtk::CellRendererState /*flags*/)
			{
				Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
				Cairo::RefPtr<Cairo::ImageSurface> ptr(aw::sprite().get_sprite(display::get_path(m_property_type.get_value(), m_property_color.get_value())));

				const int begin_x = cell_area.get_x()+(cell_area.get_width()/2)-ptr->get_width()/2;
				const int begin_y = cell_area.get_y()+(cell_area.get_height()/2)-ptr->get_height()/2;

				cr->rectangle(cell_area.get_x(), cell_area.get_y(), cell_area.get_width(), cell_area.get_height());
				cr->clip();

				if(property_sensitive() != false)
				{
					cr->set_source(ptr, begin_x, begin_y);
					cr->paint();
				}
				else
				{
					cr->set_source(ptr, begin_x, begin_y);
					cr->paint();
					cr->set_source_rgba(0, 0, 0, 0.4);
					cr->mask(ptr, begin_x, begin_y);
				}
			}

		private:
			Glib::Property<unit::types> m_property_type;
			Glib::Property<unit::colors> m_property_color;
	};
}

bool unit_view::on_button_press_event(GdkEventButton *event)
{
	bool return_value = TreeView::on_button_press_event(event);

	if((event->type == GDK_BUTTON_PRESS) && (event->button == 1))
	{
		Gtk::TreeModel::iterator iter = this->get_selection()->get_selected();
		if(iter)
		{
			Gtk::TreeModel::Row row = *iter;

			m_signal_selection_changed(row);
		}
	}

	return return_value;
}

buy_menu::model_columns::model_columns()
{
	this->add(affordable);
//	this->add(icon);
	this->add(name);
	this->add(price);
	this->add(type);
	this->add(color);
}

buy_menu::buy_menu()
{
	this->set_title("Buy Menu");
	this->set_deletable(false);

	Glib::RefPtr<Gnome::Glade::Xml> xml = Gnome::Glade::Xml::create("data/gui/buy_menu.glade", "root_vbox");

	Gtk::VBox *root_vbox = NULL;
	xml->get_widget("root_vbox", root_vbox);

	m_buy_button = NULL;
	xml->get_widget("buy_button", m_buy_button);

	Gtk::Button *cancel_button = NULL;
	xml->get_widget("cancel_button", cancel_button);
	cancel_button->signal_clicked().connect(sigc::mem_fun(*this, &buy_menu::cancel));

	xml->get_widget_derived("treeview", m_unit_view);
	xml->get_widget("funds_label", m_funds_label);

	m_unit_view->on_selection_changed().connect(boost::bind(&buy_menu::row_click_callback, this, _1));

	unit_cellrenderer *cell = new unit_cellrenderer;
	int col_count = m_unit_view->append_column("Unit", *cell);

	Gtk::TreeViewColumn *c = m_unit_view->get_column(col_count-1);
	if(c)
	{
		c->add_attribute(cell->property_type(), m_columns.type);
		c->add_attribute(cell->property_color(), m_columns.color);
	}

	m_unit_view->append_column("Name", m_columns.name);
	m_unit_view->append_column("Price", m_columns.price);

	m_list = Gtk::ListStore::create(m_columns);

	m_unit_view->set_model(m_list);

	m_unit_view->get_selection()->set_select_function(sigc::mem_fun(*this, &buy_menu::select_callback));
	m_unit_view->get_selection()->unselect_all();

	this->add(*root_vbox);

//	m_unit_view = Gtk::manage(new unit_view);
//	m_funds_label = Gtk::manage(new Gtk::Label());
//	Gtk::Button *cancel_button = Gtk::manage(new Gtk::Button("Cancel"));
//
//	cancel_button->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &Gtk::Dialog::response), Gtk::RESPONSE_CANCEL));
//
//	this->set_title("Buy Menu");
//
//	m_buy_button.set_label("Buy!");
//	m_buy_button.set_sensitive(false);
//
//	m_unit_view->on_selection_changed().connect(boost::bind(&buy_menu::row_click_callback, this, _1));
//
//
//	unit_cellrenderer *cell = new unit_cellrenderer;
//	int col_count = m_unit_view->append_column("Unit", *cell);
//
//	Gtk::TreeViewColumn *c = m_unit_view->get_column(col_count-1);
//	if(c)
//	{
//		c->add_attribute(cell->property_type(), m_columns.type);
//		c->add_attribute(cell->property_color(), m_columns.color);
//	}
//
//	m_unit_view->append_column("Name", m_columns.name);
//	m_unit_view->append_column("Price", m_columns.price);
//
//	m_list = Gtk::ListStore::create(m_columns);
//
//	m_unit_view->set_model(m_list);
//
//	m_unit_view->get_selection()->set_select_function(sigc::mem_fun(*this, &buy_menu::select_callback));
//	m_unit_view->get_selection()->unselect_all();
//
//	this->get_vbox()->pack_start(*m_funds_label);
//	this->get_vbox()->pack_start(*m_unit_view);
//	this->get_action_area()->pack_start(m_buy_button);
//	this->get_action_area()->pack_start(*cancel_button);

}

aw::gui::response_t buy_menu::run()
{
	m_return = false;

	this->show_all();

	Glib::RefPtr<Glib::MainContext> c = Glib::MainContext::get_default();

	while(!m_return)
	{
		c->iteration(true);
	}

	this->hide();

	return aw::gui::OK;
}

void buy_menu::set_workshop(unit::workshops w)
{
	aw::gui::buy_menu::set_workshop(w);

	m_list->clear();

	BOOST_FOREACH(const aw::unit_loader::value_type &pair, aw::unit_list())
	{
		const unit_loader::loaded_unit &loaded = pair.second;

		if(!(loaded.unit->is_dummy()) && loaded.produced_in == w)
		{
			Gtk::TreeModel::iterator iter = m_list->append();
			Gtk::TreeModel::Row row = *iter;

			row[m_columns.affordable] = loaded.price <= m_funds;
			row[m_columns.name] = loaded.name;
			row[m_columns.price] = loaded.price;
			row[m_columns.type] = loaded.internal_name;
			row[m_columns.color] = m_color;
		}
	}

	BOOST_FOREACH(Gtk::TreeViewColumn *column, m_unit_view->get_columns())
	{
		if(column)
		{
			column->add_attribute(column->get_first_cell_renderer()->property_sensitive(), m_columns.affordable);
		}
	}
}

void buy_menu::row_click_callback(Gtk::TreeModel::Row r)
{
	const unit::types t = r[m_columns.type];

	if(r[m_columns.price] <= m_funds)
	{
		m_buy_button->set_sensitive(true);

		std::ostringstream s;
		s << m_funds << " - " << r[m_columns.price] << " = " << (m_funds - r[m_columns.price]);

		m_funds_label->set_text(s.str());

		if(m_unit_connection.connected())
			m_unit_connection.disconnect();

		m_unit_connection = m_buy_button->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &buy_menu::buy), t));
	}
}

bool buy_menu::select_callback(const Glib::RefPtr<Gtk::TreeModel>& model, const Gtk::TreeModel::Path& path, bool)
{
	const Gtk::TreeModel::iterator iter = model->get_iter(path);

	if((*iter)[m_columns.price] <= m_funds)
		return true;
	else
		return false;
}

void buy_menu::buy(unit::types t)
{
	aw::gui::buy_menu::buy(t);
	m_return = true;
}

void buy_menu::cancel()
{
	this->hide();
	m_return = true;
}


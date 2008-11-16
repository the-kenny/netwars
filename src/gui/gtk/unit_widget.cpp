#include "unit_widget.h"
#include "gui/sprites.h"
#include "gui/paths.h"


using namespace aw::gui::gtk;

 unit_widget::unit_widget()
{
	this->set_size_request(16, 16);
}

bool unit_widget::on_expose_event(GdkEventExpose *event)
{
	bool ret = Gtk::Widget::on_expose_event(event);

	Cairo::RefPtr<Cairo::Context> cr = this->get_window()->create_cairo_context();

	if(m_unit)
	{
		this->draw(cr, aw::gui::get_path(m_unit->type(), m_unit->color()));

		int life = m_unit->get_hp();
		if(life < 10 && life > 0)
			this->draw(cr, aw::gui::get_path(unit::LIVE, life));

		if(m_unit->low_ammo())
			this->draw(cr, aw::gui::get_path(unit::LOW_AMMO));

		if(m_unit->low_fuel())
			this->draw(cr, aw::gui::get_path(unit::LOW_FUEL));

		if(m_unit->is_hidden())
			this->draw(cr, aw::gui::get_path(unit::HIDDEN));

		if(m_unit->is_transporter() && boost::dynamic_pointer_cast<transporter>(m_unit)->loaded_units_count() > 0)
			this->draw(cr, aw::gui::get_path(unit::LOADED));
	}

	return ret;
}

void unit_widget::draw(Cairo::RefPtr<Cairo::Context> &context, const std::string &filename)
{
	sprites::ptr surface = aw::sprite().get_sprite(filename);
	context->set_source(surface, 0, 0);
	context->paint();
}


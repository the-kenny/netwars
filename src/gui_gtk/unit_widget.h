#ifndef AW_GTK_UNIT_WIDGET_H
	#define AW_GTK_UNIT_WIDGET_H

#include <gtkmm.h>

#include "game/units/units.h"
#include "game/gui/unit_widget.h"

namespace aw
{
	namespace gui
	{
		namespace gtk
		{
			class unit_widget: public Gtk::DrawingArea, public aw::gui::unit_widget
			{
				public:
					unit_widget();

				protected:
					virtual bool on_expose_event(GdkEventExpose *event);

				private:
					void draw(Cairo::RefPtr<Cairo::Context> &context, const std::string &filename);
			};
		}
	}
}

#endif

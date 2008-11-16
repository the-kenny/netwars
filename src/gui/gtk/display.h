#ifndef AW_GUI_GTK_DISPLAY_H
	#define AW_GUI_GTK_DISPLAY_H

#include <gtkmm.h>
#include <cairomm/cairomm.h>
#include <boost/signals.hpp>

#include "game/scene.h"
#include "gui/display.h"

namespace aw
{
	namespace gui
	{
		namespace gtk
		{			
			class map_widget: public gui::map_widget, public Gtk::DrawingArea
			{
				public:
					map_widget();

					virtual void write_to_png(const std::string &filename);

				private:
					virtual void queue_draw();

					//Callbacks
					virtual bool on_expose_event(GdkEventExpose* event);
					virtual bool on_button_press_event(GdkEventButton* event);
					virtual bool on_motion_notify_event(GdkEventMotion *event);


					void draw(Cairo::RefPtr<Cairo::Context> &context, int x, int y, const std::string &filename);
					void update_display(Cairo::RefPtr<Cairo::Context> &cr);

					Cairo::RefPtr<Cairo::ImageSurface> m_background;
			};
		}
	}
}


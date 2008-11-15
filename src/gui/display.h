#ifndef AW_DISPLAY_H
	#define AW_DISPLAY_H

#include <gtkmm.h>
#include <cairomm/cairomm.h>
#include <boost/signals.hpp>

#include "game/scene.h"

namespace aw
{
	namespace display
	{
		//TODO: Make it bastract for different guis
		class map_widget: public Gtk::DrawingArea
		{
			public:
//				typedef boost::signal<void (int, int, int)> signal_clicked_t;
//				typedef boost::signal<void (int, int)> signal_focus_changed_t;

				typedef boost::signal<void (const coord &, int)> signal_clicked_t;
				typedef boost::signal<void (const coord &)> signal_focus_changed_t;

				typedef boost::shared_ptr<map_widget> ptr;

				map_widget();
				void display(const scene::ptr &p);
				void reset();

				void write_to_png(const std::string &filename);

				signal_clicked_t &on_click() { return m_signal_click; }
//				signal_clicked_t &on_left_click() { return m_signal_right_click; }

				signal_focus_changed_t &on_focus_changed() { return m_signal_focus_changed; }

			protected:
				virtual bool on_expose_event(GdkEventExpose* event);
				virtual bool on_button_press_event(GdkEventButton* event);
				virtual bool on_motion_notify_event(GdkEventMotion *event);


				void draw(Cairo::RefPtr<Cairo::Context> &context, int x, int y, const std::string &filename);
				void update_display(Cairo::RefPtr<Cairo::Context> &cr);

			private:
				scene::ptr m_scene;
				Cairo::RefPtr<Cairo::ImageSurface> m_background;
//				Cairo::RefPtr<Cairo::Surface> m_cached_scene;


				signal_clicked_t m_signal_click;
				signal_focus_changed_t m_signal_focus_changed;
		};

		bool is_connectable(terrain::types lhs, terrain::types rhs);
	}
}

#endif

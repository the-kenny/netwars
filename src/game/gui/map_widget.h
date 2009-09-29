#ifndef AW_DISPLAY_H
#define AW_DISPLAY_H

#include <boost/signals2/signal.hpp>

#include "game/scene.h"

namespace aw
{
  namespace gui
  {
	class map_widget
	{
	public:
	  typedef boost::signals2::signal<void 
									  (const coord &, int)> signal_clicked_t;
	  typedef boost::signals2::signal<void 
									  (const coord &)> signal_focus_changed_t;
	
	  typedef boost::shared_ptr<map_widget> ptr;

	  map_widget();
	  virtual ~map_widget() {}
	  void display(const scene::ptr &p);
	  void reset();

	  void write_to_png(const std::string &filename);
	  virtual void queue_draw() = 0;
				
	  virtual void disable() = 0;
	  virtual void enable() = 0;

	  signal_clicked_t &on_click() { return m_signal_click; }
	  signal_focus_changed_t &on_focus_changed() { return m_signal_focus_changed; }

	protected:
	  scene::ptr m_scene;
	  signal_clicked_t m_signal_click;
	  signal_focus_changed_t m_signal_focus_changed;
	};
  }
}

#endif

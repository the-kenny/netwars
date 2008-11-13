#ifndef AW_GTK_GUI_UNIT_UNLOAD_MENU_H
	#define AW_GTK_GUI_UNIT_UNLOAD_MENU_H

#include "units/unit_base.h"

#include <gtkmm/window.h>

#include "gui/unit_unload_menu.h"

namespace aw
{
	namespace gui
	{
		namespace gtk
		{
			class unit_unload_menu: public Gtk::Window, public aw::gui::unit_unload_menu
			{
				public:
					unit_unload_menu();
					int run();
					void on_button_click(int a);

					virtual void set_units(const std::vector<unit::ptr> &vec);

				private:
					volatile bool m_return;
					volatile int m_return_value;
			};
		}
	}
}

#endif

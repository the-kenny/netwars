#ifndef AW_GTK_UNIT_ACTIONS_MENU_H
	#define AW_GTK_UNIT_ACTIONS_MENU_H

#include <gtkmm/window.h>
#include <gtkmm/box.h>

#include "gui/unit_action_menu.h"

namespace aw
{
	namespace gui
	{
		namespace gtk
		{
			class unit_action_menu: public Gtk::Window, public aw::gui::unit_action_menu
			{
				public:
					unit_action_menu();
					virtual actions run();

					void add_action(actions a);

				private:
					void on_button_click(actions a)
					{
						m_return_value = a;
						m_return = true;
					}

					bool m_return;
					volatile actions m_return_value;

					Gtk::VBox *m_vbox;
			};
		}
	}
}

#endif

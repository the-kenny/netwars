#ifndef AW_UNIT_ACTIONS_MENU_H
	#define AW_UNIT_ACTIONS_MENU_H

#include <gtkmm/window.h>
#include <gtkmm/box.h>

namespace aw
{
	namespace gui
	{
		class unit_action_menu
		{
			public:
				enum actions { ATTACK, LAUNCH, EXPLODE, WAIT, CAPTURE, LOAD, UNLOAD, HIDE, APPEAR, JOIN, SUPPLY, REPAIR, CANCEL };

				unit_action_menu();

				virtual actions run() = 0;
				virtual void add_action(actions a) = 0;

			private:
				void on_button_click(actions a)
				{
					m_return_value = a;
					m_return = true;
				}

				bool m_return;
				volatile actions m_return_value;
		};
	}
}

#endif

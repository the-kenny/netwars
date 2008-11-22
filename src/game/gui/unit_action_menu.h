#ifndef AW_UNIT_ACTIONS_MENU_H
	#define AW_UNIT_ACTIONS_MENU_H

#include "game/units/actions.h"
#include <list>
#include <string>

namespace aw
{
	namespace gui
	{
		class unit_action_menu
		{
			public:
				unit_action_menu(const std::list<units::actions>& actions);
				virtual ~unit_action_menu();

				virtual ~unit_action_menu() {}

				virtual units::actions run() = 0;
				virtual void add_action(units::actions a) = 0;

				static std::string get_name(units::actions a);
				
			private:
				void on_button_click(units::actions a)
				{
					m_return_value = a;
					m_return = true;
				}

				bool m_return;
				volatile units::actions m_return_value;
		};
	}
}

#endif

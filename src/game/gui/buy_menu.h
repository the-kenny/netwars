#ifndef AW_BUY_MENU_H
	#define AW_BUY_MENU_H

#include <boost/signals2/signal.hpp>

#include "game/units/unit_base.h"
#include "game/player.h"
#include "game/units/actions.h"

#include "constants.h"

namespace aw
{
	namespace gui
	{
		class buy_menu
		{
			public:
				buy_menu(unit::workshops shop, const player::ptr &player);
				virtual ~buy_menu() {}
				virtual unit::types run() = 0;

			protected:
				virtual void buy(unit::types t);

				int m_funds;
				unit::colors m_color;
				unit::workshops m_workshop;
				
				unit::types m_unit;
		};
	}
}

#endif

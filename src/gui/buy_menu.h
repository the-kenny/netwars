#ifndef AW_BUY_MENU_H
	#define AW_BUY_MENU_H

#include <gtkmm.h>

#include <boost/signals.hpp>

#include "units/unit_base.h"
#include "player.h"

#include "constants.h"

namespace aw
{
	namespace gui
	{
		class buy_menu
		{
			public:
				typedef boost::signal<void (unit::types)> signal_buy_t;

//				virtual void add_unit(const unit::ptr &u) = 0;
				void set_player(const player::ptr &p);
				virtual void set_workshop(unit::workshops w);

				signal_buy_t &on_buy_signal() { return m_on_buy_signal; }

				virtual response_t run() = 0;

			protected:
				virtual void buy(unit::types t);

				int m_funds;
				unit::colors m_color;
				unit::workshops m_workshop;

				signal_buy_t m_on_buy_signal;

			private:

		};
	}
}

#endif

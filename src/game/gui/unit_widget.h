#ifndef AW_UNIT_WIDGET_H
	#define AW_UNIT_WIDGET_H

#include "game/units/units.h"

namespace aw
{
	namespace gui
	{
		class unit_widget
		{
			public:
				unit::ptr get_unit() const { return m_unit; }
				void set_unit(const unit::ptr &u) { m_unit = u; }

			protected:
				unit::ptr m_unit;
		};
	}
}

#endif

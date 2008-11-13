#ifndef AW_UNITS_ALL_H
	#define AW_UNITS_ALL_H

#include "unit_base.h"
#include "transporter.h"
#include "unit_loader.h"

namespace aw
{
	namespace units
	{
		inline unit::ptr create(unit::types t, unit::colors c)
		{
			return unit_list().get_unit_info(t).create_unit(c);
		}
	}
}

#endif

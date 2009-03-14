#ifndef AW_UNIT_LOADER_H
	#define AW_UNIT_LOADER_H

#include "unit_base.h"
#include "transporter.h"

#include "game/utility.h"
#include "game/terrain.h"

#include <boost/shared_ptr.hpp>

namespace aw
{
	class unit_loader: public utility::singleton<unit_loader>
	{
		public:
			class loaded_unit;

		private:
			typedef std::map<std::string, loaded_unit> container_t;

		public:
			typedef std::pair<std::string, loaded_unit> value_type;
			typedef container_t::iterator iterator;
			typedef container_t::const_iterator const_iterator;

			iterator begin()
			{
				return m_units.begin();
			}

			iterator end()
			{
				return m_units.end();
			}


			class loaded_unit
			{
				public:
					unit::ptr unit;

					std::string name;
					std::string internal_name;

					int price;
					int id;
					unit::workshops produced_in;

					std::string filename;

					unit::ptr create_unit(unit::colors c) const
					{
						unit::ptr p;

						if(unit->is_transporter())
						{
							p = unit::ptr(new aw::transporter(*boost::dynamic_pointer_cast<aw::transporter>(unit)));
						}
						else if(unit->is_dummy())
						{
							p = unit::ptr(new aw::dummy_unit(*boost::dynamic_pointer_cast<aw::dummy_unit>(unit)));
						}
						else
						{
							p = unit::ptr(new aw::unit(*unit));
						}

						p->m_color = c;
						return p;
					}
			};

			unit_loader();
			const loaded_unit &get_unit_info(const std::string &internal_name) const;

		private:
			container_t m_units;
			std::vector<std::string> m_unit_names;
	};

	unit_loader &unit_list();
}

#endif // AW_UNIT_LOADER_H

#ifndef AW_TRANSPORTER_UNIT_H
	#define AW_TRANSPORTER_UNIT_H

#include "unit_base.h"

namespace aw
{
	class transporter: public unit
	{
		public:
			typedef boost::shared_ptr<transporter> ptr;

			friend class aw::unit_loader; //ugly

			transporter(unit::types type, unit::colors color, unit::move_types mtype = unit::TIRES)
			: unit(type, color, mtype)
			{

			}

			virtual void reset_move()
			{
				unit::reset_move();

				BOOST_FOREACH(unit::ptr &p, m_loaded_units)
					p->reset_move();
			}

			bool load_unit(unit::ptr p)
			{
				if(m_loaded_units.size() < m_max_load && std::find(m_loadable_types.begin(), m_loadable_types.end(), p->type()) != m_loadable_types.end())
				 {
					m_loaded_units.push_back(p);

					return true;
				 }
				 else
				 {
					return false;
				 }
			}

			unit::ptr unload(std::size_t index)
			{
					std::vector<unit::ptr>::iterator it =  m_loaded_units.begin()+index;
					unit::ptr p = *it;
					m_loaded_units.erase(it);
					return p;
			}

			unit::ptr get_unit(std::size_t index)
			{
					std::vector<unit::ptr>::iterator it =  m_loaded_units.begin()+index;
					unit::ptr p = *it;
					return p;
			}

			unsigned int loaded_units_count()
			{
				return m_loaded_units.size();
			}

			unsigned int max_loaded_units() 
			{
				return m_max_load;
			}

			bool can_load(unit::types t)
			{
				if(m_loaded_units.size() < m_max_load && std::find(m_loadable_types.begin(), m_loadable_types.end(), t) != m_loadable_types.end())
					return true;
				else
					return false;
			}

			bool can_unload()
			{
				if(m_loaded_units.size() > 0)
					return true;
				else
					return false;
			}

			const std::vector<unit::ptr> &get_unit_list() { return m_loaded_units; }

		protected:
			unsigned int m_max_load;
			std::vector<unit::types> m_loadable_types;
			std::vector<unit::ptr> m_loaded_units;
	};
}

#endif

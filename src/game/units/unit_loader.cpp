#include "unit_loader.h"

#include <fstream>
#include "tinyxml/ticpp.h"
#include <boost/algorithm/string/replace.hpp>
#include "game/config.h"

#include "game/coord.h"

using namespace aw;

namespace
{
	unit::move_types convert_to_move_type(const std::string &t)
	{
		if(t == "foot")
			return unit::FOOT;
		else if(t == "mechanical")
			return unit::MECHANICAL;
		else if(t == "tread")
			return unit::TREAD;
		else if(t == "tires")
			return unit::TIRES;
		else if(t == "fly")
			return unit::FLY;
		else if(t == "swim")
			return unit::SWIM;
		else if(t == "transport")
			return unit::TRANSPORT;
		else if(t == "ooz")
			return unit::OOZ;
		else if(t == "pipe")
			return unit::PIPE;
		else
			throw std::runtime_error("move-type \"" + t + "\" not implented");
	}


	unit::workshops convert_to_workshop(const std::string &w)
	{
		if(w == "base")
			return unit::BASE;
		else if(w == "airport")
			return unit::AIRPORT;
		else if(w == "port")
			return unit::PORT;
		else
			throw std::runtime_error("workshop \"" + w + "\" not implented");
	}

	unit::environments convert_to_environment(const std::string &e)
	{
		if(e == "ground")
			return unit::GROUND;
		else if(e == "air")
			return unit::AIR;
		else if(e == "water")
			return unit::WATER;
		else
			throw std::runtime_error("Environment '" + e + "' not implented");
	}
}

unit_loader::unit_loader()
{
	ticpp::Document doc(aw::config().get<std::string>("/config/units").c_str());
	doc.LoadFile();

	ticpp::Iterator<ticpp::Element> iter("unit");

	for(iter = iter.begin(doc.FirstChildElement("unit_list")); iter != iter.end(); iter++)
	{
		ticpp::Element n = *iter;

		loaded_unit loaded;

		bool is_transporter = false;

		//Check if unit can transport other units
		if(n.FirstChildElement("transport", false) != NULL)
			is_transporter = true;
		else
			is_transporter = false;

		unit::ptr p;

		if(is_transporter)
		{
			p = unit::ptr(new transporter(loaded.internal_name, unit::UNDEF, unit::FOOT));
			p->m_can_load = true;
		}
		else
		{
			p = unit::ptr(new unit(loaded.internal_name, unit::UNDEF, unit::FOOT));
			p->m_can_load = false;
		}

		//Fill the attributes
		int int_data = -1;
		std::string string_data = "";
		bool bool_data = false;

		loaded.name = n.GetAttribute("name");
		loaded.internal_name = n.GetAttribute("internal_name");
		p->m_type = loaded.internal_name;

		n.GetAttribute("hp", &int_data);
		p->m_max_life = int_data;

		loaded.filename = n.GetAttribute("pixmap");

		n.GetAttribute("id", &int_data);
		loaded.id = int_data;

		//<fabrication>
		try
		{
			ticpp::Element *fabrication = n.FirstChildElement("fabrication");

			loaded.produced_in = convert_to_workshop(fabrication->GetAttribute("factory"));

			fabrication->GetAttribute("price", &int_data);
			loaded.price = int_data;

		}
		catch(const ticpp::Exception &e)
		{
			loaded.produced_in = unit::NOWHERE;
			loaded.price = -1;
		}

		//<movement>
		try
		{
			ticpp::Element *movement = n.FirstChildElement("movement");

			movement->GetAttribute("range", &int_data);
			p->m_movement_range = int_data;

			p->m_move_type = convert_to_move_type(movement->GetAttribute("type"));

			movement->GetAttributeOrDefault("capture_buildings", &bool_data, false);
			p->m_can_capture = bool_data;

			movement->GetAttributeOrDefault("can_hide", &bool_data, false);
			p->m_can_hide = bool_data;
		}
		catch(const ticpp::Exception &e)
		{
			p->m_movement_range = 0;
			p->m_can_capture = false;
			p->m_can_hide = false;
		}

		//<fuel>
		try
		{
			ticpp::Element *fuel = n.FirstChildElement("fuel");

			fuel->GetAttribute("level", &int_data);
			p->m_max_fuel = int_data;

			fuel->GetAttributeOrDefault("waste", &int_data, 0);
			p->m_fuel_per_day = int_data;

			fuel->GetAttributeOrDefault("remove_without", &bool_data, false);
			p->m_remove_without_fuel = bool_data;

			if(p->m_can_hide)
			{
				fuel->GetAttributeOrDefault("waste_hidden", &int_data, p->m_fuel_per_day);
					p->m_fuel_per_day_hidden = int_data;
			}
		}
		catch(const ticpp::Exception &e)
		{
			p->m_max_fuel = -1;
			p->m_fuel_per_day = 0;
			p->m_remove_without_fuel = false;

			if(!p->m_can_hide)
				p->m_fuel_per_day_hidden = p->m_fuel_per_day;
		}

		//<supply>
		try
		{
			ticpp::Element *supply_element = n.FirstChildElement("supply");
			p->m_can_supply = true;

			supply_element->GetAttributeOrDefault("repair_points", &int_data, 0);

			p->m_repair_points = int_data;
			if(int_data != 0)
				p->m_can_repair = true;

			ticpp::Iterator<ticpp::Element> environment_iter("environment");

			for(environment_iter = environment_iter.begin(supply_element); environment_iter != environment_iter.end(); environment_iter++)
				p->m_supply_types.push_back(convert_to_environment(environment_iter->GetAttribute("type")));
		}
		catch(const ticpp::Exception &e)
		{
			p->m_can_supply = false;
			p->m_supply_types.clear();

			p->m_can_repair = false;
			p->m_repair_points = -1;
		}

		//<combat>
		try
		{
			ticpp::Element *combat_element = n.FirstChildElement("combat");

			combat_element->GetAttributeOrDefault("counterattack", &bool_data, false);
			p->m_can_counterattack = bool_data;

			combat_element->GetAttributeOrDefault("knock_down", &bool_data, false);
			p->m_run_over = bool_data;

			//<explosive_charge>
			try
			{
				ticpp::Element *explosive_charge = combat_element->FirstChildElement("explosive_charge");

				p->m_can_explode = true;

				explosive_charge->GetAttribute("damage",&int_data);
				p->m_explosion_damage = int_data;

				explosive_charge->GetAttribute("range",&int_data);
				p->m_explosion_range = int_data;
			}
			catch(const ticpp::Exception &e)
			{
				p->m_can_explode = false;
				p->m_explosion_damage = 0;
				p->m_explosion_range = 0;
			}

			//<main_weapon>
			try
			{
				ticpp::Element *main_weapon = combat_element->FirstChildElement("main_weapon");

				p->main_weapon.active = true;

				main_weapon->GetAttribute("ammo", &int_data);
				p->main_weapon.max_ammo = int_data;

				main_weapon->GetAttribute("range", &int_data);
				p->main_weapon.range = int_data;

				main_weapon->GetAttribute("distance", &int_data);
				p->main_weapon.distance = int_data;
			}
			catch(const ticpp::Exception &e)
			{
				p->main_weapon.active = false;
				p->main_weapon.max_ammo = 0;
				p->main_weapon.range = 0;
				p->main_weapon.distance = 0;
			}

			//<alt_weapon>
			try
			{
				ticpp::Element *alt_weapon = combat_element->FirstChildElement("alt_weapon");

				p->alt_weapon.active = true;

				alt_weapon->GetAttribute("ammo", &int_data);
				p->alt_weapon.max_ammo = int_data;

				alt_weapon->GetAttribute("range", &int_data);
				p->alt_weapon.range = int_data;

				alt_weapon->GetAttribute("distance", &int_data);
				p->alt_weapon.distance = int_data;
			}
			catch(const ticpp::Exception &e)
			{
				p->alt_weapon.active = false;
				p->alt_weapon.max_ammo = 0;
				p->alt_weapon.range = 0;
				p->alt_weapon.distance = 0;
			}
		}
		catch(const ticpp::Exception &e)
		{
			p->m_run_over = false;
			p->main_weapon.active = false;
			p->alt_weapon.active = false;
		}

		if(is_transporter)
		{
			try
			{
				transporter::ptr t_p = boost::dynamic_pointer_cast<transporter>(p);

				ticpp::Element *transport_element = n.FirstChildElement("transport");

				transport_element->GetAttribute("count", &int_data);
				t_p->m_max_load = int_data;

				if(!t_p)
					throw std::runtime_error("<transport> exist, but unit is not a transporter");

				ticpp::Iterator<ticpp::Element> transportable_iter("transportable");

				for(transportable_iter = transportable_iter.begin(transport_element); transportable_iter != transportable_iter.end(); transportable_iter++)
					t_p->m_loadable_types.push_back(transportable_iter->GetAttribute("type"));
			}
			catch(const ticpp::Exception &e)
			{
				transporter::ptr t_p = boost::dynamic_pointer_cast<transporter>(p);
				t_p->m_max_load = 0;
				t_p->m_loadable_types.clear();
			}
		}

		p->reset();
		loaded.unit = p;

		m_units.insert(std::make_pair(loaded.internal_name, loaded));
		m_unit_names.push_back(loaded.internal_name);
	}


	ticpp::Iterator<ticpp::Element> dummy_iter("dummy_unit");

	for(dummy_iter = dummy_iter.begin(doc.FirstChildElement("unit_list")); dummy_iter != dummy_iter.end(); dummy_iter++)
	{
		ticpp::Element n = *dummy_iter;

		loaded_unit loaded;

		loaded.name = n.GetAttribute("name");
		loaded.internal_name = n.GetAttribute("internal_name");

		assert(loaded.internal_name != "");

		unit::ptr p(new dummy_unit(loaded.internal_name));

		int int_data = -1;

		n.GetAttribute("hp", &int_data);
		p->m_max_life = int_data;

		//TODO: Implent damage_multiplier
//		//<combat>
//		try
//		{
//			ticpp::Element *combat = n.GetElement("combat");
//
//			combat->GetAttribute
//		}
//		catch(const ticpp::Exception &e)
//		{
//
//		}

		p->reset();
		loaded.unit = p;

		m_units.insert(std::make_pair(loaded.internal_name, loaded));
		m_unit_names.push_back(loaded.internal_name);
	}
}


const unit_loader::loaded_unit &unit_loader::get_unit_info(const std::string &internal_name) const
{
	container_t::const_iterator it = m_units.find(internal_name);
	if(it != m_units.end())
		return it->second;
	else
		throw std::runtime_error("Unit '" + internal_name + "' not found.");
}

unit_loader &aw::unit_list()
{
	return unit_loader::instance();
}

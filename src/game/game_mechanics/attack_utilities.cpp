#include "attack_utilities.h"
#include "tinyxml/ticpp.h"

#include <fstream>
#include <string>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/replace.hpp>

using namespace aw::game_mechanics;
using namespace aw;

namespace
{
	std::string damagetable = "";

	void load_damagetable()
	{
		std::string s;
		std::ifstream f(config().get<std::string>("/config/damagetable").c_str());

		f.seekg(0, std::ios::end);
		std::size_t size = f.tellg();
		f.seekg(0, std::ios::beg);

		s.reserve(size);

		std::string line;

		while(std::getline(f, line))
			s += line;

		damagetable = s;
	}
}

int attack_utilities::get_damage_percent(unit::types att, unit::types vic, bool alt_fire)
{
	if(damagetable.empty())
		load_damagetable();

	ticpp::Document xml;
	xml.Parse(damagetable);

	const std::string att_name(unit::get_name(att));
	const std::string vic_name(unit::get_name(vic));

//	std::cout << "Attacker-Name: " << att_name << "\n"
//				<< "Victim-Name: " 	<< vic_name << std::endl;

	try
	{
		ticpp::Element *element = xml.FirstChildElement("damagetable");

		element = element->FirstChildElement("unit");

		try
		{
			for(; element->GetAttribute("name") != att_name; element = element->NextSiblingElement("unit"));
		}
		catch(const ticpp::Exception &e)
		{
			throw std::runtime_error("[attack_utilities::get_damage_percent] Unit '" + att + "' not found");
		}

		ticpp::Element *enemy = element->FirstChildElement("enemy");

		try
		{
			for(; enemy->GetAttribute("name") != vic_name; enemy = enemy->NextSiblingElement("enemy"));
		}
		catch(const ticpp::Exception &e)
		{
			//att can't attack vic
			return -1;
		}

		int d = -1;

		try
		{
			if(alt_fire)
				enemy->GetAttribute("alt_damage", &d);
			else
				enemy->GetAttribute("damage", &d);
		}
		catch(const ticpp::Exception &e) //Not found
		{
//			std::cout << "[In get_damage_percent()] ticpp::Exception caught: " << e.what() << std::endl;
			return -1;
		}

		if(d < -1)
			throw std::runtime_error("[In get_damage_percent()] Error with the damagetable: Damage < -1");

		return d;
	}
	catch(const ticpp::Exception &e)
	{
		std::cout << "[In get_damage_percent()] ticpp::Exception caught: " << e.what() << std::endl;
		return -1;
	}
}

bool attack_utilities::can_attack(const unit::ptr &attacker, const unit::ptr &victim)
{
	if(damagetable.empty())
		load_damagetable();

	assert(attacker != NULL);
	assert(victim != NULL);

	if(attacker->color() == victim->color())
		return false;

	float damage = -1;

//	if(attacker->main_weapon_useable())
//	{
//		damage = attack_utilities::get_damage_percent(attacker->type(), victim->type(), false);
//
//		if(damage == -1 && attacker->alt_weapon_useable())
//		{
//			damage = attack_utilities::get_damage_percent(attacker->type(), victim->type(), true);
//		}
//	}
//	else if(attacker->alt_weapon_useable())
//	{
//		damage = attack_utilities::get_damage_percent(attacker->type(), victim->type(), true);
//	}
//	else
//	{
//		throw std::runtime_error("No weapon can fire");
//	}

	int weapon = choose_weapon(attacker, victim);

	if(weapon == 0)
		damage = attack_utilities::get_damage_percent(attacker->type(), victim->type(), false);
	else if(weapon == 1)
		damage = attack_utilities::get_damage_percent(attacker->type(), victim->type(), true);
	else if(weapon == -1)
		damage = -1;

	if(damage != -1)
		return true;
	else
		return false;
}

int attack_utilities::choose_weapon(const unit::ptr &attacker, const unit::ptr &victim)
{
	if(attacker->main_weapon_useable() && attack_utilities::get_damage_percent(attacker->type(), victim->type(), false) != -1)
	{
		return 0;
	}
	else if(attacker->alt_weapon_useable() && attack_utilities::get_damage_percent(attacker->type(), victim->type(), true) != -1)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}



#include "damage_calc.h"
#include "attack_utilities.h"

#include "../config.h"
#include "fire_range.h"
#include "coord.h"

#include "../tinyxml/ticpp.h"

#include <fstream>
#include <cmath>

using namespace aw::game_mechanics;

//TODO: Rewrite damage calculation to get the damage as float
void damage_calc::calculate(const map::ptr &map, coord att, coord def)
{
	if(!map->on_map(att.x, att.y) || !map->on_map(def.x, def.y))
		throw std::runtime_error("Übergebene Koordinaten nicht im bereich der Karte");

	unit::ptr unit1 = map->get_unit(att.x, att.y);
	const terrain::ptr terrain1 = map->get_terrain(att.x, att.y);

	const float terrain1_defense = terrain1->defense_value();
	const float hp1 = unit1->get_hp();

	unit::ptr unit2 = map->get_unit(def.x, def.y);
	const terrain::ptr terrain2 = map->get_terrain(def.x, def.y);

	const float terrain2_defense = terrain2->defense_value();
	const float hp2 = unit2->get_hp();

	if(!attack_utilities::can_attack(unit1, unit2))
		throw std::runtime_error("Units können sich nicht angreifen.");

	std::cout << "[damage_calc::calculate] Terrain1-Defense: " << terrain1_defense << "\n"
				<< "[damage_calc::calculate] Terrain2-Defense: " << terrain2_defense << "\n";

	float damage = 0;

	const int att_weapon = attack_utilities::choose_weapon(unit1, unit2);

	if(att_weapon == 0)
		damage = attack_utilities::get_damage_percent(unit1->type(), unit2->type(), false);
	else if(att_weapon == 1)
		damage = attack_utilities::get_damage_percent(unit1->type(), unit2->type(), true);
	else if(att_weapon  == -1)
		damage = -1;

	damage = std::floor(damage * (100 - (terrain2_defense * hp2)) * (hp1 / 10) * 100 / 10000);
	std::cout << "[damage_calc::calculate] " << unit::get_name(unit1->type()) << "(" << hp1 << ")" << " --> " << unit::get_name(unit2->type()) << "(" << hp2 << ")" << "\tSchaden: " << damage << "%" << std::endl;

	if(static_cast<int>(damage) % 10 > 0)
	{
		if(static_cast<int>(damage) % 10 > 5)
		{
			damage = std::ceil(damage/10)*10;
		}
		else if(static_cast<int>(damage) % 10 < 5)
		{
			damage = std::floor(damage/10)*10;
		}
		else if(static_cast<int>(damage) % 10 == 5)
		{
			const int random = std::rand();
			if(random%2 == 1)
				damage = std::ceil(damage/10)*10;
			else
				damage = std::floor(damage/10)*10;
		}
	}

	unit1->fire();

	m_victim_damage = (damage / 10);
	float new_hp2 = hp2 - m_victim_damage;

	if(new_hp2 < 0)
		new_hp2 = 0;

	// COUNTERATTACK
	if(new_hp2 <= 0 || !unit2->can_counterattack() || (!unit2->main_weapon_useable() && !unit2->alt_weapon_useable()))
	{
		damage = 0;
	}
	else
	{
		fire_range f;
		f.calculate(map, def.x, def.y);

		//Check if Unit is in range for Counterattack
		if(f.get_coordinates().contains(att))
		{
			const int def_weapon = attack_utilities::choose_weapon(unit2, unit1);

			if(def_weapon == 0)
				damage = attack_utilities::get_damage_percent(unit2->type(), unit1->type(), false);
			else if(def_weapon == 1)
				damage = attack_utilities::get_damage_percent(unit2->type(), unit1->type(), true);
			else if(def_weapon == -1)
				damage = -1;

			if(damage != -1)
				unit2->fire();

			damage = std::floor(damage * (100 - (terrain1_defense * hp1)) * (new_hp2 / 10) * 100 / 10000);
			std::cout << "[damage_calc::calculate] " << unit::get_name(unit1->type()) << "(" << hp1 << ")" << " <-- " << unit::get_name(unit2->type()) << "(" << new_hp2 << ")" << "\tSchaden: " << damage << "%" << std::endl;

			if(static_cast<int>(damage) % 10 > 0)
			{
				if(static_cast<int>(damage) % 10 > 5)
				{
					damage = std::ceil(damage/10)*10;
				}
				else if(static_cast<int>(damage) % 10 < 5)
				{
					damage = std::floor(damage/10)*10;
				}
				else if(static_cast<int>(damage) % 10 == 5)
				{
					const int random = std::rand();
					if(random%2 == 1)
						damage = std::ceil(damage/10)*10;
					else
						damage = std::floor(damage/10)*10;
				}
			}
		}
		else
		{
			damage = 0;
		}
	}

	m_attacker_damage = (damage / 10);
	float new_hp1 = hp1- m_attacker_damage;

	if(new_hp1 <= 0)
		new_hp1 = 0;

	m_unit1_life = new_hp1;
	m_unit2_life = new_hp2;

	std::cout << m_attacker_damage << " " << m_victim_damage << std::endl;
}


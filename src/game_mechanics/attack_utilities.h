#ifndef AW_ATTACK_UTILITIES_H
	#define AW_ATTACK_UTILITIES_H

#include "../config.h"
#include "../units/unit_base.h"

namespace aw
{
	namespace game_mechanics
	{
		namespace attack_utilities
		{
			int get_damage_percent(unit::types att, unit::types vic, bool alt_fire);
			bool can_attack(const unit::ptr &attacker, const unit::ptr &victim);
			int choose_weapon(const unit::ptr &attacker, const unit::ptr &victim);
		}
	}
}

#endif

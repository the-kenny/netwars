#include "buy_menu.h"

#include <vector>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "../game_mechanics/prices.h"
#include "../units/unit_loader.h"
#include "../units/unit_base.h"

#include "../graphics/sprites.h"
#include "../graphics/paths.h"

using namespace aw;
using namespace aw::gui;

void buy_menu::set_player(const player::ptr &p)
{
	m_funds = p->get_funds();
	m_color = p->get_unit_color();
}

void buy_menu::set_workshop(unit::workshops w)
{
	m_workshop = w;
}

void buy_menu::buy(unit::types t)
{
	m_on_buy_signal(t);
}


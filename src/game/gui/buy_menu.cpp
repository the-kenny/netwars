#include "buy_menu.h"

#include "game/units/unit_base.h"


using namespace aw;
using namespace aw::gui;

buy_menu::buy_menu(unit::workshops shop, const player::ptr &p)
{
	m_workshop = shop;
	m_funds = p->get_funds();
	m_color = p->get_unit_color();
}

void buy_menu::buy(unit::types t)
{
	m_unit = t;
}


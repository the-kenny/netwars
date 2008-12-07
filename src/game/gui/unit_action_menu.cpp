#include "unit_action_menu.h"
 #include "game/units/actions.h"

using namespace aw::gui;
using namespace aw::units;

//Initialize the variables
unit_action_menu::unit_action_menu(const std::list<units::actions>& actions)
: m_return(false), m_return_value(units::CANCEL)
{
	
}

unit_action_menu::~unit_action_menu()
{
}

std::string unit_action_menu::get_name(units::actions a)
{
	switch(a) 
	{
		case ATTACK:
			return "Attack";
		case EXPLODE:
			return "Explode";
		case LAUNCH:
			return "Launch";
		case WAIT:
			return "Wait";
		case CAPTURE:
			return "Capture";
		case LOAD:
			return "Load";
		case UNLOAD:
			return "Unload";
		case HIDE:
			return "Hide";
		case APPEAR:
			return "Appear";
		case JOIN:
			return "Join";
		case SUPPLY:
			return "Supply";
		case REPAIR:
			return "Repair";
		case CANCEL:
			return "Cancel";
		default:
			return "Unknown Action";
	}
}


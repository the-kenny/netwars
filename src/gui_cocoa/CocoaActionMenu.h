#import <Cocoa/Cocoa.h>
#import "UnitActionMenuController.h"

#include "game/gui/unit_action_menu.h"

class CocoaActionMenu: aw::gui::unit_action_menu {
public:	
	CocoaActionMenu();
	~CocoaActionMenu();
	
	//Callback-method
	static aw::units::actions showActionMenu(const std::list<aw::units::actions>& actions);
	
	virtual aw::units::actions run();
	virtual void add_action(aw::units::actions a);
	
private:
	UnitActionMenuController* controller;
};

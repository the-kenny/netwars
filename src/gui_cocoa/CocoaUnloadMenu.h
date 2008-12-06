//
//  CocoaUnloadMenu.h
//  gui_cocoa
//
//  Created by Moritz Ulrich on 06.12.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "UnitUnloadMenuController.h"

#include "game/gui/unit_unload_menu.h"
#include "game/units/unit_base.h"

class CocoaUnloadMenu: aw::gui::unit_unload_menu {
public:
	CocoaUnloadMenu();
	~CocoaUnloadMenu();
	
	//Callback-method
	static int showUnloadMenu(const std::list<aw::unit::ptr>& units);
	
	virtual int run();
	virtual void set_units(const std::list<aw::unit::ptr> &vec);
	
private:
	UnitUnloadMenuController* controller;
};


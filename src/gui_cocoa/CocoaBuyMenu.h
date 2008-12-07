//
//  CocoaBuyMenu.h
//  gui_cocoa
//
//  Created by Moritz Ulrich on 07.12.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "BuyMenuController.h"

#include "game/gui/buy_menu.h"


class CocoaBuyMenu: aw::gui::buy_menu {
public:
	CocoaBuyMenu(aw::unit::workshops shop, const aw::player::ptr &player);
	~CocoaBuyMenu();
	virtual aw::unit::types run();
	
	//Callback-method
	static aw::unit::types showBuyMenu(aw::unit::workshops shop, const aw::player::ptr &player);
	
private:
	BuyMenuController* controller;
};
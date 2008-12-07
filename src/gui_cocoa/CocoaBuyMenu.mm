//
//  CocoaBuyMenu.m
//  gui_cocoa
//
//  Created by Moritz Ulrich on 07.12.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "CocoaBuyMenu.h"

CocoaBuyMenu::CocoaBuyMenu(aw::unit::workshops shop, const aw::player::ptr &player)
: aw::gui::buy_menu(shop, player) {
	controller = [[BuyMenuController alloc] initWithPlayer:player workshop:shop];
}

CocoaBuyMenu::~CocoaBuyMenu() {
	[controller release];
}

aw::unit::types CocoaBuyMenu::run() {
	return [controller run];
}

aw::unit::types CocoaBuyMenu::showBuyMenu(aw::unit::workshops shop, const aw::player::ptr &player) {
	CocoaBuyMenu b(shop, player);
	return b.run();
}
//
//  CocoaActionMenu.m
//  gui_cocoa
//
//  Created by Moritz Ulrich on 06.12.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "CocoaActionMenu.h"

CocoaActionMenu::CocoaActionMenu()
: unit_action_menu(std::list<aw::units::actions>()) {
	controller = [[UnitActionMenuController alloc] init];
}

CocoaActionMenu::~CocoaActionMenu() {
	[controller release];
}

aw::units::actions CocoaActionMenu::run() {
	return [controller run:[Coordinate coordinateWithPoint:[NSEvent mouseLocation]]];
}

void CocoaActionMenu::add_action(aw::units::actions a) {
	[controller addAction:a];
}
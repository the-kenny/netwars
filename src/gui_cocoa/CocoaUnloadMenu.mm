//
//  CocoaUnloadMenu.mm
//  gui_cocoa
//
//  Created by Moritz Ulrich on 06.12.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "CocoaUnloadMenu.h"

CocoaUnloadMenu::CocoaUnloadMenu() {
	controller = [[UnitUnloadMenuController alloc] init];
}

CocoaUnloadMenu::~CocoaUnloadMenu() {
	[controller release];
}

int CocoaUnloadMenu::showUnloadMenu(const std::list<aw::unit::ptr>& units) {
	CocoaUnloadMenu m;
	m.set_units(units);
	return m.run();
}

int CocoaUnloadMenu::run() {
	return [controller run:[NSEvent mouseLocation]];
}

void CocoaUnloadMenu::set_units(const std::list<aw::unit::ptr> &vec) {
	[controller setUnits:vec];
}

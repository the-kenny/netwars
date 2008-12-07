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

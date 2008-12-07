//
//  AppController.m
//  gui_cocoa
//
//  Created by Moritz Ulrich on 19.11.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "AppController.h"
#import "MapView.h"
#import "Coordinate.h"
#import "CocoaActionMenu.h"
#import "CocoaUnloadMenu.h"
#import "CocoaBuyMenu.h"
#import "GameDialogController.h"

#include "game/config.h"
#include "game/units/actions.h"


#include <string>
#include <list>
#include <boost/bind.hpp>

@implementation AppController

@synthesize gameActive;

- (void)awakeFromNib {
	NSLog(@"AppController.awakeFromNib");
		
	//Ugly ugly hack to locate config.xml and $datadir in the resource dir of an appbundle
	NSBundle *main = [NSBundle mainBundle];
	aw::config().load(std::string([[main pathForResource:@"config" ofType:@"xml"] UTF8String]));
	aw::config().set("/config/dirs/data", std::string([main.resourcePath UTF8String]) + "/" + aw::config::instance().get<std::string>("/config/dirs/data", false));
	
	 //gameController = aw::game_controller::ptr(new aw::game_controller);
	 //gameController->signal_scene_change().connect(boost::bind(&aw::gui::map_widget::display, cocoaMapWidget, _1));
	
	mainWindowController = [[MainWindowController alloc] initWithWindowNibName:@"MainWindow"];
	[[mainWindowController window] makeMainWindow];
	[[mainWindowController window] makeKeyAndOrderFront:self];
	
	 [[NSNotificationCenter defaultCenter] 
	 addObserver:self 
	 selector:@selector(mouseClickOnMap:) 
	 name: leftMouseClickNotification
	 object:nil];
	
	[[NSNotificationCenter defaultCenter] 
	 addObserver:self 
	 selector:@selector(rightMouseClickOnMap:) 
	 name: rightMouseClickNotification
	 object:nil];
	
	[[NSNotificationCenter defaultCenter] 
	 addObserver:self 
	 selector:@selector(mouseMovedOnMap:) 
	 name: mouseMovedNotification
	 object:nil];
	
	[self setGameActive:NO];
}

#pragma mark "Event Handling"

- (void)mouseClickOnMap:(NSNotification*)notification {
	Coordinate* coord = [notification.userInfo objectForKey:@"Position"];
	gameController->click(coord.coord, 1);
}

- (void)rightMouseClickOnMap:(NSNotification*)notification {
	Coordinate* coord = [notification.userInfo objectForKey:@"Position"];
	//NSLog(@"rightMouseClockOnMap: (%i|%i)", coord.coord.x, coord.coord.y);
	gameController->click(coord.coord, 3);
}

- (void)mouseMovedOnMap:(NSNotification*)notification {
	Coordinate* coord = [notification.userInfo objectForKey:@"Position"];
	//NSLog(@"mouseMoved: (%i|%i)", coord.coord.x, coord.coord.y);
	gameController->mouse_hover_changed(coord.coord);
}

#pragma mark "Game related stuff"

- (void)initGame {
	mapView = mainWindowController.mapView;
	
	cocoaMapWidget = CocoaMapWidget::ptr(new CocoaMapWidget(mapView));
}

- (IBAction)startNewGame:(id)sender {
	GameDialogController* gameDialog = [[GameDialogController alloc] init];
	
	if([gameDialog run]) {
		[self initGame];
		
		aw::game::ptr game(new aw::game);

		game->load_map(std::string([gameDialog.mapFile UTF8String]));
		game->set_funds_per_building(gameDialog.fundsPerBuilding);
		game->set_initial_funds(gameDialog.initialFunds);
		
		gameController = aw::game_controller::ptr(new aw::game_controller);
		gameController->signal_scene_change().connect(boost::bind(&aw::gui::map_widget::display, cocoaMapWidget, _1));
		gameController->signal_show_unit_action_menu().connect(boost::bind(&CocoaActionMenu::showActionMenu, _1));
		gameController->signal_show_unload_menu().connect(boost::bind(&CocoaUnloadMenu::showUnloadMenu, _1));
		gameController->signal_show_buy_menu().connect(boost::bind(&CocoaBuyMenu::showBuyMenu, _1, _2));
		
		[mapView setIsEnabled:YES];
		[self setGameActive:YES];
		
		gameController->start_game(game);
	}
}

- (IBAction)endTurn:(id)sender {
	gameController->end_turn();
	gameController->start_turn();
}
@end

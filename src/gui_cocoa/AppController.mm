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
#import "NotificationNames.h"
#import "CocoaUnit.h"
#import "CocoaTerrain.h"

#include "game/config.h"
#include "game/units/actions.h"


#include <string>
#include <list>
#include <boost/bind.hpp>

void gameFinished(id game, const aw::player::ptr& p) {
  [game gameFinished:p];
}

@implementation AppController

@synthesize gameActive;

- (void)awakeFromNib {		
	//Ugly ugly hack to locate config.xml and $datadir in the resource dir of an appbundle
	NSBundle *main = [NSBundle mainBundle];
	aw::config().load(std::string([[main pathForResource:@"config" ofType:@"xml"] UTF8String]));
	aw::config().set("/config/dirs/data", std::string([main.resourcePath UTF8String]) + "/" + aw::config::instance().get<std::string>("/config/dirs/data", false));
	
	 //gameController = aw::game_controller::ptr(new aw::game_controller);
	 //gameController->signal_scene_change().connect(boost::bind(&aw::gui::map_widget::display, cocoaMapWidget, _1));
	
	mainWindowController = [[MainWindowController alloc] initWithWindowNibName:@"MainWindow"];
	[[mainWindowController window] makeMainWindow];
	[[mainWindowController window] makeKeyAndOrderFront:self];
	
	mapView = [mainWindowController.mapView retain];
		
	 [[NSNotificationCenter defaultCenter] 
	 addObserver:self 
	 selector:@selector(mouseClickOnMap:) 
	 name: leftMouseClickNotification
	 object:mapView];
	
	[[NSNotificationCenter defaultCenter] 
	 addObserver:self 
	 selector:@selector(rightMouseClickOnMap:) 
	 name: rightMouseClickNotification
	 object:mapView];
	
	[[NSNotificationCenter defaultCenter] 
	 addObserver:self 
	 selector:@selector(mouseMovedOnMap:) 
	 name: mouseMovedNotification
	 object:mapView];

	//Bind isEnabled of MapView to self.gameActive
	[mapView bind:@"isEnabled" 
			 toObject:self
			 withKeyPath:@"gameActive"
			 options:nil];
	
	//Set the delegate
	[NSApp setDelegate:self];
	
	[self setGameActive:NO];
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] 
	 removeObserver:self];
	
	[mainWindowController release];
	
	[mapView release];
	
	[super dealloc];
}

- (void)gameFinished:(const aw::player::ptr&)winner {
  [self setGameActive:false];

  NSRunAlertPanel(@"Game has ended", [NSString stringWithFormat:@"The %@ player has won!",
											   [NSString stringWithCString:
														   winner->get_color_string().c_str()]],
				  nil, nil, nil);
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

#pragma mark Event Creating

void postUnitClickedNotification(const aw::unit::ptr& unit, id sender) {
	NSDictionary* userInfo = [NSDictionary dictionaryWithObject:[CocoaUnit cocoaUnitWithUnit:unit] forKey:@"unit"];
	[[NSNotificationCenter defaultCenter] postNotificationName:unitClickedNotification
														object:sender
													  userInfo:userInfo];
}

void postTerrainClickedNotification(const aw::terrain::ptr terrain, id sender) {
	NSDictionary* userInfo = [NSDictionary dictionaryWithObject:[CocoaTerrain cocoaTerrainWithTerrain:terrain] forKey:@"terrain"];
	[[NSNotificationCenter defaultCenter] postNotificationName:terrainClickedNotification
														object:sender
													  userInfo:userInfo];
}

#pragma mark "Game related stuff"

- (void)initGame {
	cocoaMapWidget = CocoaMapWidget::ptr(new CocoaMapWidget(mapView));
}


- (void)startNewGameImpl:(GameDialogController*)gameDialog {
	[self initGame];
	
	aw::game::ptr game(new aw::game);
	
	game->load_map(std::string([gameDialog.mapFile UTF8String]));
	game->set_funds_per_building(gameDialog.fundsPerBuilding);
	game->set_initial_funds(gameDialog.initialFunds);
	game->signal_game_finished().connect(boost::bind(&gameFinished, self, _1));
	
	
	gameController = aw::game_controller::ptr(new aw::game_controller);
	gameController->signal_scene_change().connect(boost::bind(&aw::gui::map_widget::display, cocoaMapWidget, _1));
	gameController->signal_show_unit_action_menu().connect(boost::bind(&CocoaActionMenu::showActionMenu, _1));
	gameController->signal_show_unload_menu().connect(boost::bind(&CocoaUnloadMenu::showUnloadMenu, _1));
	gameController->signal_show_buy_menu().connect(boost::bind(&CocoaBuyMenu::showBuyMenu, _1, _2));
	
	//Connect general events
	gameController->signal_unit_clicked().connect(boost::bind(&postUnitClickedNotification, _1, self));
	gameController->signal_terrain_clicked().connect(boost::bind(&postTerrainClickedNotification, _1, self));
	
	[mapView setIsEnabled:YES];
	[self setGameActive:YES];
	
	gameController->start_game(game);
}

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename {
	GameDialogController* gameDialog = [[GameDialogController alloc] init];
	[gameDialog setMapFile:filename];
	
	if([gameDialog run]) {
		[self startNewGameImpl:gameDialog];
		
		return YES;
	}
	
	[gameDialog release];
	
	return NO;
}

- (IBAction)startNewGame:(id)sender {
	GameDialogController* gameDialog = [[GameDialogController alloc] init];
	
	if([gameDialog run]) {
		[self startNewGameImpl:gameDialog];
	}
	
	[gameDialog release];
}

- (IBAction)endTurn:(id)sender {
	gameController->end_turn();
	gameController->start_turn();
}
@end

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

#import "game/config.h"

#import <string>
#include <boost/bind.hpp>

@implementation AppController

- (void)awakeFromNib {
	NSLog(@"AppController.awakeFromNib");
		
	//Ugly ugly hack to locate config.xml and $datadir in the resource dir of an appbundle
	NSBundle *main = [NSBundle mainBundle];
	aw::config().load(std::string([[main pathForResource:@"config" ofType:@"xml"] UTF8String]));
	aw::config().set("/config/dirs/data", std::string([main.resourcePath UTF8String]) + "/" + aw::config::instance().get<std::string>("/config/dirs/data", false));
	
	 gameController = aw::game_controller::ptr(new aw::game_controller);
	 gameController->signal_scene_change().connect(boost::bind(&aw::gui::map_widget::display, cocoaMapWidget, _1));
	
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
	 
}

#pragma mark "Event Handling"

- (void)mouseClickOnMap:(NSNotification*)notification {
	Coordinate* coord = [notification.userInfo objectForKey:@"Position"];
			
	gameController->click(coord.coord, 1);
}

- (void)rightMouseClickOnMap:(NSNotification*)notification {
	Coordinate* coord = [notification.userInfo objectForKey:@"Position"];
	 	
	gameController->click(coord.coord, 2);
}

- (void)mouseMovedOnMap:(NSNotification*)notification {
	NSLog(@"mouseMoved");
	Coordinate* coord = [notification.userInfo objectForKey:@"Position"];

	
	std::cout << coord.coord.x << " " << coord.coord.y << std::endl;
	
	gameController->mouse_hover_changed(coord.coord);
}

#pragma mark "Game related stuff"

- (void)startNewGame {
	
}
@end

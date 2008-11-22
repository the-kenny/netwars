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
	NSLog(@"awakeFromNib");
		
	//Ugly ugly hack to locate config.xml and $datadir in the resource dir of an appbundle
	NSBundle *main = [NSBundle mainBundle];
	aw::config().load(std::string([[main pathForResource:@"config" ofType:@"xml"] UTF8String]));
	aw::config().set("/config/dirs/data", std::string([main.resourcePath UTF8String]) + "/" + aw::config::instance().get<std::string>("/config/dirs/data", false));
	
	gameController = aw::game_controller::ptr(new aw::game_controller);
	//cocoaMapWidget = CocoaMapWidget::ptr(new CocoaMapWidget(mapView));
	gameController->signal_scene_change().connect(boost::bind(&aw::gui::map_widget::display, cocoaMapWidget, _1));
	
	mainWindowController = [[NSWindowController alloc] initWithWindowNibName:@"MainWindow"];
	[[mainWindowController window] makeMainWindow];
	[[mainWindowController window] makeKeyAndOrderFront:self];
	 
		
	[[NSNotificationCenter defaultCenter] 
	 addObserver:self 
	 selector:@selector(mouseDown:) 
	 name: @"mouseClickOnMap" 
	 object:mapView];
	
	[[NSNotificationCenter defaultCenter] 
	 addObserver:self 
	 selector:@selector(rightMouseDown:) 
	 name: @"rightMouseClickOnMap" 
	 object:mapView];
	
	[[NSNotificationCenter defaultCenter] 
	 addObserver:self 
	 selector:@selector(mouseMoved:) 
	 name: @"mouseMoveOnMap" 
	 object:mapView];
}

- (void)mouseDown:(NSNotification*)notification {
	Coordinate* coord = [notification.userInfo objectForKey:@"Position"];
			
	gameController->click(coord.coord, 1);
}

- (void)rightMouseDown:(NSNotification*)notification {
	Coordinate* coord = [notification.userInfo objectForKey:@"Position"];
	 	
	gameController->click(coord.coord, 2);
}

- (void)mouseMoved:(NSNotification*)notification {
	NSLog(@"mouseMoved");
	Coordinate* coord = [notification.userInfo objectForKey:@"Position"];

	
	std::cout << coord.coord.x << " " << coord.coord.y << std::endl;
	
	gameController->mouse_hover_changed(coord.coord);
}
@end

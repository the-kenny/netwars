//
//  main.m
//  gui_cocoa
//
//  Created by Moritz Ulrich on 16.11.08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "game/gui/paths.h"
#import "game/config.h"

#import <string>

int main(int argc, char *argv[])
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	//Ugly ugly hack to locate config.xml and $datadir in the rsesource dir of an appbundle
	NSBundle *main = [NSBundle mainBundle];
	aw::config::instance().load(std::string([[main pathForResource:@"config" ofType:@"xml"] UTF8String]));
	aw::config::instance().set("/config/dirs/data", std::string([main.resourcePath UTF8String]) + "/" + aw::config::instance().get<std::string>("/config/dirs/data", false));
	
	std::cout << aw::gui::get_path("tank", aw::unit::RED);
    return NSApplicationMain(argc,  (const char **) argv);
} 

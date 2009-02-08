//
//  Sprites.mm
//  gui_cocoa
//
//  Created by Moritz Ulrich on 22.11.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "Sprites.h"

#include "game/gui/paths.h"


@implementation Sprites
SYNTHESIZE_SINGLETON_FOR_CLASS(Sprites)

-(Sprites*)init {
	self = [super init];
	
	//We allocate it, and we have to deallocate it (see dealloc)
	cache = [[NSMutableDictionary alloc] init];
	
	return self;
}

-(void)dealloc {
	[cache release];
	[super dealloc];
}

-(NSImage*)getSprite:(NSString*)filename {
	id value = [cache objectForKey:filename];
	if(value != nil) {
		return [(NSImage*)value autorelease];
	} else {
		NSImage* image = [[NSImage alloc] initWithContentsOfFile:filename];
		[cache setObject:image forKey:filename];
		[image release];
		
		return [image autorelease];
	}
	
}
@end

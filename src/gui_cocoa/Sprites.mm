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
	
	cache = [NSMutableDictionary dictionary];
	
	return self;
}

-(NSImage*)getSprite:(NSString*)filename {
	id value = [cache valueForKey:filename];
	if(value != nil) {
		return (NSImage*)value;
	} else {
		NSImage* image = [[NSImage alloc] initWithContentsOfFile:filename];
		[cache setObject:image forKey:filename];
		
		return image;
	}
	
}
@end

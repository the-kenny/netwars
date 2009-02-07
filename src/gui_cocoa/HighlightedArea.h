//
//  HighlightedArea.h
//  gui_cocoa
//
//  Created by Moritz Ulrich on 07.02.09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "game/area.h"


@interface HighlightedArea : NSObject {
	CALayer* layer;
	__strong aw::area highlightedArea;
	
	NSImage* sprite;
}

@property CALayer* layer;
@property(readwrite) aw::area highlightedArea;

- (id)init;
- (id)initWithSprite:(NSImage*)s;
- (void)dealloc;

- (void)draw;
- (void)drawHighlightAt:(NSPoint)pos;

@end

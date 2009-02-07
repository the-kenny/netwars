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
	aw::area highlightedArea;
}

@property(assign) aw::area highlightedArea;

- (void)draw;
- (void)drawHighlightAt:(NSPoint)pos;

@end

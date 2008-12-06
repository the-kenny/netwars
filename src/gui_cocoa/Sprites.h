//
//  Sprites.h
//  gui_cocoa
//
//  Created by Moritz Ulrich on 22.11.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "SynthesizeSingleton.h"


@interface Sprites : NSObject {
	NSMutableDictionary* cache;
}

+(Sprites*)sharedSprites;
-(NSImage*)getSprite:(NSString*)filename;

@end

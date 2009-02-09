//
//  CocoaTerrain.h
//  gui_cocoa
//
//  Created by Moritz Ulrich on 09.02.09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "game/terrain.h"

@interface CocoaTerrain : NSObject {
	__strong aw::terrain::ptr terrain;
}

- (id)initWithTerrain:(aw::terrain::ptr)t;

+ (CocoaTerrain*)cocoaTerrainWithTerrain:(aw::terrain::ptr)u;


@property aw::terrain::ptr terrain;


@end

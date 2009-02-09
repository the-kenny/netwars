//
//  CocoaTerrain.mm
//  gui_cocoa
//
//  Created by Moritz Ulrich on 09.02.09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "CocoaTerrain.h"


@implementation CocoaTerrain

@dynamic terrain;

- (aw::terrain::ptr)terrain {
	return terrain;
}

- (void)setTerrain:(aw::terrain::ptr)t {
	terrain = t;
}

- (id)initWithTerrain:(aw::terrain::ptr)t {
	self = [super init];
	
	terrain = t;
	
	return self;
}

+ (CocoaTerrain*)cocoaTerrainWithTerrain:(aw::terrain::ptr)t {
	CocoaTerrain* cocoaTerrain = [[CocoaTerrain alloc] initWithTerrain:t];
	
	return [cocoaTerrain autorelease];
}

@end

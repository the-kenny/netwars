//
//  CocoaUnit.mm
//  gui_cocoa
//
//  Created by Moritz Ulrich on 09.02.09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "CocoaUnit.h"


@implementation CocoaUnit

@dynamic unit;

- (aw::unit::ptr)unit {
	return unit;
}

- (void)setUnit:(aw::unit::ptr)u {
	unit = u;
}

- (id)initWithUnit:(aw::unit::ptr)u {
	self = [super init];
	
	unit = u;
	
	return self;
}

+ (CocoaUnit*)cocoaUnitWithUnit:(aw::unit::ptr)u {
	CocoaUnit* cocoaUnit = [[CocoaUnit alloc] initWithUnit:u];
	
	return [cocoaUnit autorelease];
}

@end

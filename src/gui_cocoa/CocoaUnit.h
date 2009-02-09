//
//  CocoaUnit.h
//  gui_cocoa
//
//  Created by Moritz Ulrich on 09.02.09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "game/units/unit_base.h"

@interface CocoaUnit : NSObject {
	 aw::unit::ptr unit;
}

- (id)initWithUnit:(aw::unit::ptr)u;

+ (CocoaUnit*)cocoaUnitWithUnit:(aw::unit::ptr)u;


@property aw::unit::ptr unit;

@end

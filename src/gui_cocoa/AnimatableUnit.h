#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

#include "game/units/unit_base.h"


@interface AnimatableUnit : NSObject {
	aw::unit::ptr unit;
}

@property(readwrite) aw::unit::ptr unit;

- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx;

@end

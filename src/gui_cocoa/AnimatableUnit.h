#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

#include "game/units/unit_base.h"


@interface AnimatableUnit : NSObject {
	__strong aw::unit::ptr unit;
	CALayer* layer;
}

@property aw::unit::ptr unit;
@property CALayer* layer;

- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx;

- (void)draw;

@end

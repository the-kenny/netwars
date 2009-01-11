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
- (void)animationDidStop:(CAAnimation *)theAnimation finished:(BOOL)flag;

- (void)setPosition:(NSPoint)p;
- (void)moveTo:(NSPoint)p;


- (void)draw;
- (void)drawSprite:(NSString*)filename;

@end

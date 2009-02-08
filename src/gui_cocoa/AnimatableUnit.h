#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

#include "game/units/unit_base.h"
#include "game/terrain.h"


@interface AnimatableUnit : NSObject {
	__strong aw::unit::ptr unit;
	CALayer* layer;
	
	//Hack for displaying the capture-status
	__strong aw::terrain::ptr currentTerrain;
}

@property aw::unit::ptr unit;
@property CALayer* layer;

//Hack
@property aw::terrain::ptr currentTerrain;

//- (void)animationDidStop:(CAAnimation *)theAnimation finished:(BOOL)flag;

- (void)setPosition:(NSPoint)p;
- (void)moveTo:(NSPoint)p;


- (void)draw;
- (void)drawSprite:(NSString*)filename;

@end

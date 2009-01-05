#import <Cocoa/Cocoa.h>

#include "game/scene.h"

extern NSString* mouseMovedNotification;
extern NSString* leftMouseClickNotification;
extern NSString* rightMouseClickNotification;

@interface MapView : NSView {
	NSTrackingArea* trackingArea;
	
	aw::scene::ptr currentScene;
	
	NSImage* background;
	NSImage* maskImage;
	
	NSMutableSet* managedUnits;
	NSArray* unitMovements;
	
	bool isEnabled;
}

@property(readwrite) bool isEnabled;
@property(readwrite) aw::scene::ptr scene;

- (NSPoint)toViewCoordinates:(NSPoint)pos rect:(NSSize)size;
- (NSPoint)toGameCoordinates:(NSPoint)pos;
- (void)addUnitForDrawing:(aw::unit::ptr)u at:(NSPoint)at;

- (void)drawTerrain;

- (void)queueDraw;

- (NSArray*)unitMovements:(aw::scene::ptr&)newScene;
@end


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
	
	bool isEnabled;
}

@property(readwrite) bool isEnabled;
@property(readwrite) aw::scene::ptr scene;

- (NSPoint)toViewCoordinates:(NSPoint)pos rect:(NSSize)size;
- (NSPoint)toGameCoordinates:(NSPoint)pos;

- (void)drawTerrain;

- (void)queueDraw;

- (NSArray*)unitMovements:(aw::scene::ptr&)newScene;
@end


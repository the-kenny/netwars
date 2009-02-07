#import <Cocoa/Cocoa.h>

#import "AnimatableUnit.h"
#import "HighlightedArea.h"

#include "game/scene.h"

#include <map>

extern NSString* mouseMovedNotification;
extern NSString* leftMouseClickNotification;
extern NSString* rightMouseClickNotification;

@interface MapView : NSView {
	NSTrackingArea* trackingArea;
	
	aw::scene::ptr currentScene;
	
	NSImage* background;
	NSImage* maskImage;
	
	NSMutableSet* managedUnits;
	std::map<aw::unit::ptr, AnimatableUnit*> unitMap;
	NSArray* unitActions;
	
	HighlightedArea* highlightedArea;
	HighlightedArea* highlightedPath;
	
	bool isEnabled;
}

@property(readwrite) bool isEnabled;
@property(readwrite) aw::scene::ptr scene;

- (NSPoint)toViewCoordinates:(NSPoint)pos rect:(NSSize)size;
- (NSPoint)toGameCoordinates:(NSPoint)pos;

- (void)addUnitForDrawing:(aw::unit::ptr)u at:(NSPoint)at;
- (void)removeUnitFromDrawing:(aw::unit::ptr)u;

- (void)drawTerrain;

- (void)queueDraw;

- (void)processScene:(aw::scene::ptr&)newScene;
@end


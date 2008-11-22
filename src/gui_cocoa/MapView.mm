#import "MapView.h"
#import "Coordinate.h"
#import "Sprites.h"

#include "game/gui/paths.h"

#include <iostream>

@implementation MapView

@synthesize scene = currentScene;

- (void)awakeFromNib {
	NSLog(@"MapView.awakeFromNib");
	trackingArea = [[NSTrackingArea alloc] initWithRect:NSMakeRect(0.0, 0.0, 480.0, 320.0) 
											options:NSTrackingMouseMoved|NSTrackingActiveInActiveApp 
											owner:self userInfo:nil];
	[self addTrackingArea:trackingArea];
	
	currentScene = aw::scene::ptr(new aw::scene);
}

- (void)mouseDown:(NSEvent *)theEvent {
	NSPoint point = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	point.x /= 16;
	point.y /= 16;
	point.y = abs(point.y-19);

	[[NSNotificationCenter defaultCenter] 
	 postNotificationName:@"mouseClickOnMap" object:self
	 userInfo: [NSDictionary dictionaryWithObject:[Coordinate coordinateWithPoint:point] forKey:@"Position"]];
}

- (void)rightMouseDown:(NSEvent *)theEvent {
	NSPoint point = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	point.x /= 16;
	point.y /= 16;
	point.y = abs(point.y-19);
	
	[[NSNotificationCenter defaultCenter] 
	 postNotificationName:@"rightMouseClickOnMap" object:self
	 userInfo: [NSDictionary dictionaryWithObject:[Coordinate coordinateWithPoint:point] forKey:@"Position"]];
}

- (void)mouseMoved:(NSEvent *)theEvent {
	NSLog(@"Mousemoved");
	
	NSPoint actualPoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	
	static int old_x;               
	static int old_y; 
	
	int x = (actualPoint.x+theEvent.deltaX)/16;
	int y = (actualPoint.y+theEvent.deltaY)/16;
	
	y = abs(y-19);
		
	if((x != old_x || y != old_y) && (x < 30 && y < 20))
	{
		 [[NSNotificationCenter defaultCenter] 
		 postNotificationName:@"mouseMoveOnMap" object:self
		  userInfo: [NSDictionary dictionaryWithObject:[Coordinate coordinateWithCoordinates:x y:y] forKey:@"Position"]];
		
		old_x = x;
		old_y = y;
	}
}

- (void)drawRect:(NSRect)rect { 
	NSGraphicsContext* context = [NSGraphicsContext currentContext]; 
	NSImage* sprite = [[Sprites sharedSprites] getSprite:[NSString stringWithCString:aw::gui::get_path("tank", aw::unit::RED).c_str()]];
	[sprite drawAtPoint:NSMakePoint(0, 0) fromRect:NSZeroRect operation:NSCompositeCopy fraction:1.0];
} 


- (void)queueDraw {
	[self setNeedsDisplay:true];
}

- (void)disable {
	
}

- (void)enable {
	
}

@end

void CocoaMapWidget::queue_draw() {
	//mapView.scene = m_scene;
	[mapView queueDraw];
}

void CocoaMapWidget::disable() {
	//mapView.scene = m_scene;
	[mapView disable];
}

void CocoaMapWidget::enable() {
	//mapView.scene = m_scene;
	[mapView enable];
}

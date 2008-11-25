#import <Cocoa/Cocoa.h>

#include "game/scene.h"
#include "game/gui/map_widget.h"

@interface MapView : NSView {
	NSTrackingArea* trackingArea;
	
	aw::scene::ptr currentScene;
	
	NSImage* background;
	NSImage* maskImage;
}

@property(readwrite) aw::scene::ptr scene;

- (void)queueDraw;
- (void)disable;
- (void)enable;
@end

class CocoaMapWidget: public aw::gui::map_widget {
public:
	typedef boost::shared_ptr<CocoaMapWidget> ptr;
	
	CocoaMapWidget(MapView* mView) {
		setView(mView);
	}
	
	void setView(MapView* mView) {
		mapView = mView;
	}
	
	virtual void queue_draw();
	
	virtual void disable();     
	virtual void enable(); 
	
private:
	MapView* mapView;
};

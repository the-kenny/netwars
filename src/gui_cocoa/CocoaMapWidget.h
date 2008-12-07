#import <Cocoa/Cocoa.h>
#import "MapView.h"

#include "game/gui/map_widget.h"

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

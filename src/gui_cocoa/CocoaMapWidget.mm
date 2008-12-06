/*
 *  CocoaMapWidget.cpp
 *  gui_cocoa
 *
 *  Created by Moritz Ulrich on 27.11.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "CocoaMapWidget.h"


void CocoaMapWidget::queue_draw() {
	mapView.scene = m_scene;
	[mapView queueDraw];
}

void CocoaMapWidget::disable() {
	mapView.scene = m_scene;
	[mapView setIsEnabled:NO];
}

void CocoaMapWidget::enable() {
	mapView.scene = m_scene;
	[mapView setIsEnabled:YES];
}
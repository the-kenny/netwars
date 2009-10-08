//
//  FlashingColorLayer.h
//  gui_cocoa
//
//  Created by Moritz Ulrich on 09.10.09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

@interface FlashingColorLayer : NSObject {
	CALayer* layer;
	NSColor* color;
}

@property CALayer* layer;

-(void)flash;

@end

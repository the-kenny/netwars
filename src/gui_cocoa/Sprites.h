#import <Cocoa/Cocoa.h>
#import "SynthesizeSingleton.h"


@interface Sprites : NSObject {
	NSMutableDictionary* cache;
}

+(Sprites*)sharedSprites;
-(NSImage*)getSprite:(NSString*)filename;

@end

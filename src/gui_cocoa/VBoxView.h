#include <Cocoa/Cocoa.h>

@interface VBoxView : NSView {
	@private
		NSMutableArray *_items;
		NSUInteger _spacing;
}

@property (assign) NSUInteger spacing;

- (void)clearItems;
- (void)removeItem:(NSView *)view;
- (void)removeItemAtIndex:(NSUInteger)index;

- (void)addItem:(NSView *)view;
- (void)insertItem:(NSView *)view atIndex:(NSUInteger)index;
- (void)insertItems:(NSArray *)views atIndexes:(NSIndexSet *)indexes;

@end


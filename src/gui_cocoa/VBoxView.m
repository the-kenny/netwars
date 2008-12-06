#import "VBoxView.h"

/* ======================================================================
 *  PRIVATE Methods of VBoxView
 */
@interface VBoxView (VBoxViewPrivate)
- (void)adjustSubviews;
- (CGFloat)currentHeight;

- (void)moveItem:(NSView *)view space:(CGFloat)space;
- (void)moveItemsFromIndex:(NSUInteger)index space:(CGFloat)space;
@end

@implementation VBoxView

@synthesize spacing = _spacing;

#pragma mark Initializing Filterbar

- (id)initWithFrame:(NSRect)frameRect {
	if ((self = [super initWithFrame:frameRect])) {
		_items = [[NSMutableArray alloc] init];
		_spacing = 4;
	}

	return self;
}

- (void)dealloc {
	[self clearItems];
	[_items release];

	[super dealloc];
}

#pragma mark Examining Coordinate System Modifications

- (BOOL)isFlipped {
	return YES;
}

#pragma mark - Removing Objects

- (void)clearItems {
	NSUInteger subviewCount = [_items count];

	while (subviewCount-- > 0) {
		NSView *view = [_items lastObject];
		[_items removeLastObject];

		[view removeFromSuperview];
		[view release];
	}

	[self setNeedsDisplay:YES];
}

- (void)removeItem:(NSView *)view {
	NSUInteger index = [_items indexOfObject:view];	
	[_items removeObject:view];

	CGFloat viewHeight = [view frame].size.height;
	[view removeFromSuperview];
	[view release];

	[self moveItemsFromIndex:index space:(-1 * viewHeight)];
	[self setNeedsDisplay:YES];
}

- (void)removeItemAtIndex:(NSUInteger)index {
	NSView *view = [_items objectAtIndex:index];
	[_items removeObjectAtIndex:index];

	CGFloat viewHeight = [view frame].size.height;
	[view removeFromSuperview];
	[view release];

	[self moveItemsFromIndex:index space:(-1 * viewHeight)];
	[self setNeedsDisplay:YES];
}

#pragma mark - Adding Items

- (void)addItem:(NSView *)view {
	if ([view respondsToSelector:@selector(sizeToFit)])
		[((id) view) sizeToFit];

	NSRect vboxFrame = [self frame];
	NSRect viewFrame = [view frame];
	NSView *lastView = [_items lastObject];
	if (lastView != nil) {
		NSRect lastViewFrame = [lastView frame];
		viewFrame.origin.y = lastViewFrame.origin.y +
							 lastViewFrame.size.height +
							 _spacing;
	} else {
		vboxFrame.size.height = 0;
		viewFrame.origin.y = [self bounds].origin.y;
	}
	
	viewFrame.origin.x = vboxFrame.origin.x;
	viewFrame.size.width = vboxFrame.size.width;
	[view setFrame:viewFrame];

	[_items addObject:view];
	[self addSubview:view];

	vboxFrame.size.height += viewFrame.size.height + _spacing;
	[self setFrame:vboxFrame];

	[self setNeedsDisplay:YES];
}

- (void)insertItem:(NSView *)view atIndex:(NSUInteger)index {
	if ([view respondsToSelector:@selector(sizeToFit)])
		[((id) view) sizeToFit];

	[_items insertObject:view atIndex:index];
	[self addSubview:view];
	[self adjustSubviews];

	[self setNeedsDisplay:YES];
}

- (void)insertItems:(NSArray *)views atIndexes:(NSIndexSet *)indexes {
	NSUInteger currentIndex = [indexes firstIndex];
	NSUInteger i, count = [indexes count];

	for (i = 0; i < count; ++i) {
		[self insertItem:[views objectAtIndex:i] atIndex:currentIndex];
		currentIndex = [indexes indexGreaterThanIndex:currentIndex];
	}
}

#pragma mark Resizing Subviews

- (void)resizeSubviewsWithOldSize:(NSSize)oldBoundsSize {
	NSSize newBoundsSize = [self bounds].size;
	
	CGFloat vboxHeight = [self currentHeight];
	if (vboxHeight != newBoundsSize.height) {
		NSRect frameRect = [self frame];
		frameRect.size.height = vboxHeight;
		[self setFrame:frameRect];
	}

	if ([self bounds].size.width != oldBoundsSize.width) {
		[self adjustSubviews];
		[self setNeedsDisplay:YES];
	}
}

/* ======================================================================
 *  Implementation of PRIVATE Methods
 */

#pragma mark Adjusting Subviews

- (void)adjustSubviews {
	NSRect vboxFrame = [self bounds];
	CGFloat y = vboxFrame.origin.y;

	for (NSView *view in _items) {
		/* Update View Position */
		NSRect viewFrame = [view frame];
		viewFrame.origin.y = y;
		viewFrame.origin.x = vboxFrame.origin.x;
		viewFrame.size.width = vboxFrame.size.width;
		[view setFrame:viewFrame];

		if ([view respondsToSelector:@selector(sizeToFit)])
			[((id) view) sizeToFit];

		/* Next Position is view.y + view.height + space */
		y += viewFrame.size.height + _spacing;
	}
}

- (CGFloat)currentHeight {
	NSView *lastView = [_items lastObject];
	if (lastView == nil)
		return([self frame].size.width);

	NSRect lastViewRect = [lastView frame];
	return(lastViewRect.size.height + lastViewRect.origin.y + _spacing);
}

- (void)moveItem:(NSView *)view space:(CGFloat)space {
	NSRect viewRect = [view frame];
	viewRect.origin.y += space;
	[view setFrame:viewRect];
}

- (void)moveItemsFromIndex:(NSUInteger)index space:(CGFloat)space {
	NSArray *subviewArray = [self subviews];
	NSUInteger subviewCount = [subviewArray count];

	for (; index < subviewCount; ++index)
		[self moveItem:[subviewArray objectAtIndex:index] space:space];
}

@end


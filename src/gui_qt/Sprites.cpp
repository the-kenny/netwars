#include "Sprites.h"

QImage Sprites::getSprite(const QString& filename) {
	const QString resourceFilename(":/" + filename);

	if(!imageCache.contains(resourceFilename)) {
		QImage image(resourceFilename);
		imageCache.insert(resourceFilename, image);

		return image;
	} else {
		return imageCache[resourceFilename];
	}	
}

Sprites& sharedSprites() {
	return Sprites::instance();
}

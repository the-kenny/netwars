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

QImage Sprites::getSprite(const std::string& filename) {
  return getSprite(filename.c_str());
}

Sprites& sharedSprites() {
	return Sprites::instance();
}

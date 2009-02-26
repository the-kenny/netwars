#ifndef AW_QT_SPRITES_H
	#define AW_QT_SPRITES_H

#include <QImage>
#include <QMap>
#include <QString>

#include <string>

#include "game/utility.h"

class Sprites: public aw::utility::singleton<Sprites> {
	public:
		QImage getSprite(const QString& filename);
		QImage getSprite(const std::string& filename);
	
	private:
		QMap<QString, QImage> imageCache;
};

Sprites& sharedSprites();

#endif

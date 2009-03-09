#ifndef AW_QT_PIXMAP_DRAWING_H
#define AW_QT_PIXMAP_DRAWING_H

#include <string>
#include <QPainter>
#include <QImage>

#include "Sprites.h"
#include "game/coord.h"

namespace Drawing { 
  void drawPixmap(const std::string& path, const aw::coord& c, QPainter& painter) {
	if(path.size() != 0) {
	  QImage sprite = sharedSprites().getSprite(QString(path.c_str()));
	  assert(!sprite.isNull());

	  QSize size = sprite.size();

	  painter.drawImage(QPoint((c.x*16)+(16-size.width()), (c.y*16)+(16-size.height())), sprite);
	}
  }

  void drawPixmap(const QImage& sprite, const aw::coord& c, QPainter& painter) {
	QSize size = sprite.size();
	painter.drawImage(QPoint((c.x*16)+(16-size.width()), (c.y*16)+(16-size.height())), sprite);
  }
}

#endif

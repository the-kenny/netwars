#include "PixmapDrawing.h"

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

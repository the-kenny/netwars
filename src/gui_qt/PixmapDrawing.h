#ifndef AW_QT_PIXMAP_DRAWING_H
#define AW_QT_PIXMAP_DRAWING_H

#include <string>
#include <QPainter>
#include <QImage>

#include "Sprites.h"
#include "game/coord.h"

namespace Drawing { 
  void drawPixmap(const std::string& path, const aw::coord& c, QPainter& painter);
  void drawPixmap(const QImage& sprite, const aw::coord& c, QPainter& painter);
}

#endif

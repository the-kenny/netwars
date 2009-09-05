#include "MinimapDrawingThread.h"

void MinimapDrawingThread::run() {
  QImage img(100,100,QImage::Format_RGB32);

	QPainter painter;

	painter.begin(&img);

	painter.fillRect(0,0,10,10,QColor("red"));

	painter.end();

	Q_EMIT minimapReady(img);
  }

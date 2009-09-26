#include "MinimapDrawingThread.h"

#include "game/map_loader/map_loader.h"

MinimapDrawingThread::MinimapDrawingThread(const std::string& mapFile,
										   QLabel* label) {
  mapFile_ = mapFile;
  label_ = label;
}

void MinimapDrawingThread::run() {
  aw::map_loader loader;
  aw::map_loader::loaded_map::ptr lm = loader.load(mapFile_);

  QRect rect = label_->frameRect();
  double rectWidth = rect.width()/30;
  double rectHeight = rect.height()/20;

  QImage img(rect.width(),rect.height(),QImage::Format_RGB32);
  
  QPainter painter;
  
  painter.begin(&img);
  
  painter.fillRect(0,0,10,10,QColor("red"));
  
  painter.end();
  
  Q_EMIT minimapReady(img);
}

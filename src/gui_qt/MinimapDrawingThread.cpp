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

  QSize rect = label_->size();
  double rectWidth = rect.width()/30;
  double rectHeight = rect.height()/20;

  QImage img(rect.width(),rect.height(),QImage::Format_RGB32);
  
  QPainter painter;
  
  painter.begin(&img);

  for(int x = 0; x < 30; x++) {
	for(int y = 0; y < 20; y++) {
	  const aw::terrain::ptr& terrain = lm->m_terrain[x][y];
	  const aw::unit::ptr& unit = lm->m_unit[x][y];

	  if(terrain) {
		QColor terrainColor;
		
		switch(terrain->type()) {
		case aw::terrain::PLAIN:
		case aw::terrain::WRECKAGE:
		  terrainColor = QColor(204, 240, 70);
		  break;
		case aw::terrain::WATER:
		case aw::terrain::SEASIDE:
		  terrainColor = QColor(40, 255, 240);
		  break;
		case aw::terrain::FOREST:
		  terrainColor = QColor(80, 160, 160);
		  break;
		case aw::terrain::REEF:
		  terrainColor = QColor(50, 110, 250);
		  break;
		case aw::terrain::MOUNTAIN:
		  terrainColor = QColor(255.0, 230, 230);
		  break;
		case aw::terrain::PIPE:
		case aw::terrain::SEGMENT_PIPE:
		  terrainColor = QColor(200, 110, 130);
		  break;
		case aw::terrain::BRIDGE:
		case aw::terrain::STREET:
		  terrainColor = Qt::gray;
		  break;
		case aw::terrain::RIVER:
		  terrainColor = QColor(80, 130, 255);
		  break;
		case aw::terrain::BEACH:
		  terrainColor = QColor(255.0, 240, 80);
		  break;
		  
		default:
		  terrainColor = Qt::white;
		}


		if(terrain->is_building()) {		
		  if(terrain->type() != aw::terrain::HEADQUARTER) {
			switch(terrain->extra()) {
			case aw::terrain::RED:
			  terrainColor = Qt::red;
			  break;
			case aw::terrain::GREEN:
			  terrainColor = Qt::green;
			  break;
			case aw::terrain::BLUE:
			  terrainColor = Qt::blue;
			  break;
			case aw::terrain::YELLOW:
			  terrainColor = Qt::yellow;
			  break;
			case aw::terrain::BLACK:
			  terrainColor = Qt::darkGray;
			  break;
			case aw::terrain::WHITE:
			  terrainColor = Qt::white;
			  break;
			}
		  } else {
			switch(terrain->extra()) {
			case aw::terrain::RED:
			  terrainColor = QColor(210, .0, .0);
			  break;
			case aw::terrain::GREEN:
			  terrainColor = QColor(.0, 210, .0);
			  break;
			case aw::terrain::BLUE:
			  terrainColor = QColor(.0, .0, 210);
			  break;
			case aw::terrain::YELLOW:
			  terrainColor = QColor(210, 210, .0);
			  break;
			case aw::terrain::BLACK:
			  terrainColor = Qt::black;
			  break;
			case aw::terrain::WHITE:
			  terrainColor = Qt::white;
			  break;
			}
		  }
		}

		painter.fillRect(x*rectWidth, y*rectHeight,
					   rectWidth, rectHeight,
					   terrainColor);
	  }
	  
	  

	  if(unit && ! unit->is_dummy()) {
		QColor unitColor;
		
		switch(unit->color()) {
		case aw::unit::RED:
		  unitColor = Qt::red;
		  break;
		case aw::unit::GREEN:
		  unitColor = Qt::green;
		  break;
		case aw::unit::BLUE:
		  unitColor = Qt::blue;
		  break;
		case aw::unit::YELLOW:
		  unitColor = Qt::yellow;
		  break;
		case aw::unit::BLACK:
		  unitColor = Qt::darkGray;
		  break;
		default:
		  unitColor = Qt::white;
		  break;
		}

		painter.fillRect(x*rectWidth, y*rectHeight,
						 rectWidth/1.8, rectHeight/1.8,
						 unitColor);
	  }
	}
  }
  
  painter.end();
  
  Q_EMIT minimapReady(img);
}

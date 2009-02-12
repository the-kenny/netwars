#ifndef AW_QT_MAP_WIDGET_H 
	#define AW_QT_MAP_WIDGET_H

#include <QWidget>
#include <QImage>

#include "game/scene.h"

class MapWidget: public QWidget {
	Q_OBJECT

	public:
		MapWidget(QWidget* parent);

		aw::scene::ptr scene() const { return currentScene; }
		void setScene(aw::scene::ptr scene) { 
			currentScene = scene; 
			this->repaint(); 
		}

	protected:
		virtual void paintEvent(QPaintEvent* event);

	private:
		void drawUnits(QPainter& painter);
		void drawTerrain(QPainter& painter);
		void drawPixmap(const std::string path, const aw::coord& coord, QPainter& painter);

		QImage backgroundImage;

		aw::scene::ptr currentScene;
};

#endif

#ifndef AW_QT_MAP_WIDGET_H 
	#define AW_QT_MAP_WIDGET_H

#include <QWidget>
#include <QImage>

#include "game/scene.h"

#include <boost/signals.hpp>

class MapWidget: public QWidget {
	Q_OBJECT

	public:
		typedef boost::signal<void (const aw::coord &, int)> signalClickedT;
		typedef boost::signal<void (const aw::coord &)> signalFocusChangedT;

		MapWidget(QWidget* parent);

		aw::scene::ptr scene() const { return currentScene; }
		void setScene(aw::scene::ptr scene) { 
			currentScene = scene; 
			this->repaint(); 
		}

		signalClickedT &signalClicked() { return _signalClicked; }
		signalFocusChangedT &signalFocusChanged() { return _signalFocusChanged; }

	protected:
		virtual void paintEvent(QPaintEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseMoveEvent(QMouseEvent* event);

	private:
		void drawUnits(QPainter& painter);
		void drawTerrain(QPainter& painter);
		void drawPixmap(const std::string path, const aw::coord& coord, QPainter& painter);

		QImage backgroundImage;

		aw::scene::ptr currentScene;
	
		//Boost signals:
		signalClickedT _signalClicked;
		signalFocusChangedT _signalFocusChanged;
};

#endif

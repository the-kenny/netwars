#include "MapWidget.h"

#include <QPainter>

#include "Sprites.h"

#include "game/gui/drawing.h"

MapWidget::MapWidget(QWidget* parent) 
	: QWidget(parent), 
	backgroundImage(":/data/background.png") {

}

void MapWidget::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

	painter.drawImage(QPoint(), backgroundImage);

	if(currentScene) {
		this->drawTerrain(painter);
	}
}

void MapWidget::drawTerrain(QPainter& painter) {
	for(int x = 0; x < 30; ++x) {
		for(int y = 0; y < 30; ++y) {
			painter.drawImage(QPoint(x*16, y*16), 
							  sharedSprites().getSprite(QString(aw::gui::get_sprite_for(
									  					aw::coord(x, y), 
														currentScene).c_str())));
		}
	}
}

#include "MapWidget.h"

#include <QPainter>

MapWidget::MapWidget(QWidget* parent) 
	: QWidget(parent), 
	backgroundImage(":/data/background.png") {

}

void MapWidget::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

	painter.drawImage(QPoint(), backgroundImage);
}

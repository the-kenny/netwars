#include "MapWidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <boost/foreach.hpp>

#include "Sprites.h"

#include "game/config.h"
#include "game/gui/drawing.h"
#include "game/gui/paths.h"

using namespace aw;

namespace {
	//Used for masking
	static QImage blackImage = QImage(32, 32, QImage::Format_ARGB32);
}

MapWidget::MapWidget(QWidget* parent) 
	: QWidget(parent), 
	backgroundImage(":/data/background.png") {
	
	//Initialize the blackImage;
	blackImage.fill(0xff000000);
}

void MapWidget::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

	painter.drawImage(QPoint(), backgroundImage);

	if(currentScene) {
		this->drawTerrain(painter);
		this->drawUnits(painter);

		const std::string& pixmapdir = aw::config().get<std::string>("/config/dirs/pixmaps");

		//Draw Path and Highlighted Area
		this->drawHighlightedArea(painter, currentScene->get_highlighted_area(), pixmapdir + "misc/range.png");
		this->drawHighlightedArea(painter, currentScene->get_path_area(), pixmapdir + "misc/path.png");

		if(currentScene->highlight())
			this->drawPixmap(pixmapdir + "misc/highlight.png",
				currentScene->highlight(), 
				painter);
	}
}

void MapWidget::mousePressEvent(QMouseEvent* event) {
	aw::coord realCoord(event->x()/16, event->y()/16);

	int button = -1;

	switch(event->button()) {
		case Qt::LeftButton:
			button = 1;
			break;
		case Qt::RightButton:
			button = 3;
			break;
		default:
			return;
	}

	_signalClicked(realCoord, button);
}

void MapWidget::mouseMoveEvent(QMouseEvent* event) {
	static coord lastCoord(0, 0);
	coord currentCoord(event->x()/16, event->y()/16);

	if(currentCoord != lastCoord) {
		_signalFocusChanged(currentCoord);
		lastCoord = currentCoord;
	}
}

void MapWidget::drawPixmap(const std::string& path, const aw::coord& c, QPainter& painter) {
	if(path.size() != 0) {
		QImage sprite = sharedSprites().getSprite(QString(path.c_str()));
		assert(!sprite.isNull());

		QSize size = sprite.size();

		painter.drawImage(QPoint((c.x*16)+(16-size.width()), (c.y*16)+(16-size.height())), sprite);
	}
}

void MapWidget::drawPixmap(const QImage& sprite, const aw::coord& c, QPainter& painter) {
		QSize size = sprite.size();
		painter.drawImage(QPoint((c.x*16)+(16-size.width()), (c.y*16)+(16-size.height())), sprite);
}


void MapWidget::drawUnits(QPainter& painter) {
	for(int x = 0; x < 30; ++x) {
		for(int y = 0; y < 20; ++y) {
			aw::unit::ptr u = currentScene->get_unit(x, y);

			if(u != NULL && !u->is_dummy()) {
				this->drawPixmap(aw::gui::get_path(u->type(), u->color()), 
						aw::coord(x, y),
						painter);

				if(u->moved()) {
					QImage mask = sharedSprites().getSprite(QString(aw::gui::get_path(u->type(), 
									u->color()).c_str())).copy();
					QPainter imagePainter(&mask);

					imagePainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
					imagePainter.drawImage(QPoint(), blackImage);

					painter.save();

					painter.setOpacity(0.4);
					this->drawPixmap(mask, aw::coord(x, y), painter);

					painter.restore();
				}

				int life = u->hp();
				if(life < 10 && life > 0)
					this->drawPixmap(gui::get_path(unit::LIVE, life), aw::coord(x, y), painter);

				if(u->low_ammo())
					this->drawPixmap(gui::get_path(unit::LOW_AMMO), aw::coord(x, y), painter);

				if(u->low_fuel())
					this->drawPixmap(gui::get_path(unit::LOW_FUEL), aw::coord(x, y), painter);

				if(u->is_hidden())
					this->drawPixmap(gui::get_path(unit::HIDDEN), aw::coord(x, y), painter);

				if(u->is_transporter() && boost::dynamic_pointer_cast<transporter>(u)->loaded_units_count() > 0)
					this->drawPixmap(gui::get_path(unit::LOADED), aw::coord(x, y), painter);

				if(u->can_capture())
				{
					const terrain::ptr &t(currentScene->get_terrain(x, y));

					if(t->is_building() && boost::dynamic_pointer_cast<building>(t)->capture_points() < 20)
					{
						this->drawPixmap(gui::get_path(unit::CAPTURE), aw::coord(x, y), painter);
					}
				}
			}
		}
	}
}

void MapWidget::drawTerrain(QPainter& painter) {
	for(int x = 0; x < 30; ++x) {
		for(int y = 0; y < 20; ++y) {
			this->drawPixmap(aw::gui::get_sprite_for(aw::coord(x, y), 
						currentScene),
					aw::coord(x, y), 
					painter);
		}
	}
}

void MapWidget::drawHighlightedArea(QPainter& painter, const aw::area& area, const std::string& pixmap) {
	BOOST_FOREACH(const coord& c, area)
		this->drawPixmap(pixmap, c, painter);
}

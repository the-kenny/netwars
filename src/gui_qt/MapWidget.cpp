#include "MapWidget.h"

#include <QPainter>

#include "Sprites.h"

#include "game/gui/drawing.h"
#include "game/gui/paths.h"

using namespace aw;

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

void MapWidget::drawPixmap(const std::string path, const aw::coord& c, QPainter& painter) {
	painter.drawImage(QPoint(c.x*16, c.y*16), 
			sharedSprites().getSprite(QString(path.c_str())));
}

void MapWidget::drawUnits(QPainter& painter) {
	for(int x = 0; x < 30; ++x) {
		for(int y = 0; y < 30; ++y) {
			aw::unit::ptr u = currentScene->get_unit(x, y);

			if(u != NULL && !u->is_dummy()) {
				this->drawPixmap(aw::gui::get_path(u->type(), u->color()), 
						aw::coord(x, y),
						painter);


				int life = u->get_hp();
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
		for(int y = 0; y < 30; ++y) {
			this->drawPixmap(aw::gui::get_sprite_for(aw::coord(x, y), 
						currentScene),
					aw::coord(x, y), 
					painter);
		}
	}
}

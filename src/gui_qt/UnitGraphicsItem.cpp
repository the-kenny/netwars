#include "UnitGraphicsItem.h"

#include "PixmapDrawing.h"
#include "Sprites.h"

#include "game/gui/drawing.h"
#include "game/gui/paths.h"

namespace {
	//Used for masking
  static QImage blackImage = QImage(32, 32, QImage::Format_ARGB32);
  bool initialized = false;

  void initialize() {
	//Initialize the blackImage;
	blackImage.fill(0xff000000);

	initialized = true;
  }
}

UnitGraphicsItem::UnitGraphicsItem(QGraphicsItem* parent)
  : QGraphicsItem(parent) {

  if(!initialized)
	initialize();
}

void UnitGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  using namespace aw;
  
  if(_unit) {
	const aw::unit::ptr& u = _unit;
	if(u != NULL && !u->is_dummy()) {
	  Drawing::drawPixmap(aw::gui::get_path(u->type(), u->color()), 
						  aw::coord(0, 0),
						  *painter);

	  const aw::coord nullCoord(0, 0);
  
	  if(u->moved()) {
		QImage mask = sharedSprites().getSprite(QString(aw::gui::get_path(u->type(), 
																		  u->color()).c_str())).copy();
		QPainter imagePainter(&mask);
		
		imagePainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
		imagePainter.drawImage(QPoint(), blackImage);
		
		painter->save();

		painter->setOpacity(0.4);
		Drawing::drawPixmap(mask, nullCoord, *painter);

		painter->restore();
	  }

	  int life = u->get_hp();
	  if(life < 10 && life > 0)
		Drawing::drawPixmap(gui::get_path(unit::LIVE, life), nullCoord, *painter);

	  if(u->low_ammo())
		Drawing::drawPixmap(gui::get_path(unit::LOW_AMMO), nullCoord, *painter);

	  if(u->low_fuel())
		Drawing::drawPixmap(gui::get_path(unit::LOW_FUEL), nullCoord, *painter);

	  if(u->is_hidden())
		Drawing::drawPixmap(gui::get_path(unit::HIDDEN), nullCoord, *painter);

	  if(u->is_transporter() && boost::dynamic_pointer_cast<transporter>(u)->loaded_units_count() > 0)
		Drawing::drawPixmap(gui::get_path(unit::LOADED), nullCoord, *painter);

	  /*
		if(u->can_capture())
		{
		  const terrain::ptr &t(currentScene->get_terrain(x, y));

		  if(t->is_building() && boost::dynamic_pointer_cast<building>(t)->capture_points() < 20) {
			  Drawing::drawPixmap(gui::get_path(unit::CAPTURE), nullCoord, *painter);
			}
		}
	  */
	}
  }
}

QRectF UnitGraphicsItem::boundingRect() const {
  //TODO: Replace with scene.size
  
  return QRectF(0.0, 0.0, 30.0*16, 20.0*16);
}

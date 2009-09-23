#include "UnitGraphicsItem.h"

#include "PixmapDrawing.h"
#include "Sprites.h"

#include <QCoreApplication>
#include <QGraphicsItemAnimation>

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
  : QGraphicsItem(parent),
	timeLine(NULL),
	animation(NULL) {

  if(!initialized)
	initialize();
}

void UnitGraphicsItem::moveTo(const QPointF& p) {
  if(timeLine) {
	if(timeLine->state() == QTimeLine::Running) {
	  //Not good... we have to wait until the animation is finished
	  //But this shouldn't happen

	  while(timeLine->state() == QTimeLine::Running)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100); 
	}
	
	//delete timeLine;
	//timeLine = NULL;
  }

  if(animation) {
	animation->reset();
	delete animation;
	animation = NULL;
  }
   
  animation = new QGraphicsItemAnimation;
  animation->setPosAt(1.0, p);
  
  timeLine = new QTimeLine;
  timeLine->setDuration(1000);
  timeLine->setLoopCount(1);
  timeLine->setUpdateInterval(1000.0/30);
  timeLine->setCurveShape(QTimeLine::EaseInOutCurve);
  
  animation->setItem(this);
  animation->setTimeLine(timeLine);

  timeLine->start();
}

void UnitGraphicsItem::stop() {
  if(animation) {
	animation->reset();
	delete animation;
	animation = NULL;
  }
}

void UnitGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  using namespace aw;
  
  if(_unit) {
	const aw::unit::ptr& u = _unit;
	if(u != NULL && !u->is_dummy()) {
	  Drawing::drawPixmap(aw::gui::get_path(u->type(), u->color()), 
						  aw::coord(0, 0),
						  *painter);

	  static const aw::coord nullCoord(0, 0);
  
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

	  
	  if(u->can_capture()) {
		if(_currentTerrain && _currentTerrain->is_building() && boost::dynamic_pointer_cast<building>(_currentTerrain)->capture_points() < 20) {
		  Drawing::drawPixmap(gui::get_path(unit::CAPTURE), 
							  nullCoord, 
							  *painter);
		}
	  } 
	}
  }
}

QRectF UnitGraphicsItem::boundingRect() const {
  //TODO: Replace with scene.size
  
  return QRectF(0.0, 0.0, 30.0*16, 20.0*16);
}

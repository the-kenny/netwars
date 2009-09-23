#ifndef AW_QT_TERRAIN_GRAPHICS_ITEM_H
#define AW_QT_TERRAIN_GRAPHICS_ITEM_H

#include <QGraphicsItem>
#include <QGraphicsItemAnimation>
#include <QTimeLine>

#include <boost/shared_ptr.hpp>

#include "game/coord.h"
#include "game/units/unit_base.h"
#include "game/terrain.h"

class UnitGraphicsItem: public QGraphicsItem {
public:
  typedef boost::shared_ptr<UnitGraphicsItem> ptr;
  
  UnitGraphicsItem(QGraphicsItem* parent = 0);

  void setUnit(const aw::unit::ptr &u) { _unit = u; }
  const aw::unit::ptr& unit() const { return _unit; }

  void setCurrentTerrain(const aw::terrain::ptr& t) { _currentTerrain = t; }
  const aw::terrain::ptr& CurrentTerrain() const { return _currentTerrain; }
	
  void moveTo(const QPointF& p);
  void stop();
  
protected:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  virtual QRectF boundingRect() const;

private:
  aw::unit::ptr _unit;

  aw::terrain::ptr _currentTerrain;

  QTimeLine* timeLine;
  QGraphicsItemAnimation* animation;
};

#endif

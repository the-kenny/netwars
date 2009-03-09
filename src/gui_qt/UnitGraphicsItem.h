#ifndef AW_QT_TERRAIN_GRAPHICS_ITEM_H
#define AW_QT_TERRAIN_GRAPHICS_ITEM_H

#include <QGraphicsItem>

#include "game/coord.h"
#include "game/units/unit_base.h"

class UnitGraphicsItem: public QGraphicsItem {
public:
  UnitGraphicsItem(QGraphicsItem* parent = 0);

  void setUnit(const aw::unit::ptr &u) { _unit = u; }
  const aw::unit::ptr& unit() const { return _unit; }
	
  void moveTo(const aw::coord& c);
  
protected:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  virtual QRectF boundingRect() const;

private:
  aw::unit::ptr _unit;
};

#endif

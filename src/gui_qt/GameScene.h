#ifndef AW_QT_MAP_WIDGET_H 
	#define AW_QT_MAP_WIDGET_H

#include <QWidget>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include <string>

#include "game/scene.h"

#include "UnitGraphicsItem.h"
#include "PathGraphicsItem.h"

#include <boost/signals.hpp>

class UnitActions {
public:
  enum Actions { REMOVED, ADDED, MOVED };

  Actions action;
  aw::coord position;
  aw::coord oldPosition;
  aw::unit::ptr unit;
};

class GameScene: public QGraphicsScene {
	Q_OBJECT

	public:
		typedef boost::signal<void (const aw::coord &, int)> signalClickedT;
		typedef boost::signal<void (const aw::coord &)> signalFocusChangedT;

		GameScene(QWidget* parent);

		aw::scene::ptr scene() const { return currentScene; }
        void setScene(aw::scene::ptr scene);


		// Resets all signals and makes the View ready for a new game
		void reset();

		signalClickedT &signalClicked() { return _signalClicked; }
  		signalFocusChangedT &signalFocusChanged() { return _signalFocusChanged; }

	protected:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
  //To resend a single click if thr user clicks too fast
        virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

  virtual void drawBackground(QPainter* painter, const QRectF& rect);
  virtual void drawItems(QPainter *painter,
						 int numItems, QGraphicsItem *items[],
						 const QStyleOptionGraphicsItem options[],
						 QWidget *widget);

	private:
  //void drawUnits(QPainter& painter);
  //	void drawHighlightedArea(QPainter& painter, const aw::area& area, const std::string& pixmap);

		QImage backgroundImage;

		aw::scene::ptr currentScene;
	
		//Boost signals:
		signalClickedT _signalClicked;
		signalFocusChangedT _signalFocusChanged;

  QPointF mapToSceneCoord(const aw::coord& c) {
	return QPointF(c.x*16,c.y*16);
  }
  
  UnitGraphicsItem* getUnitGraphicsItem(const aw::unit::ptr& u);
  void addUnitForDrawing(const aw::unit::ptr &u, const aw::coord& c);
  void removeUnitFromDrawing(const aw::unit::ptr &u);
  void processNewScene(const aw::scene::ptr& scene);

  std::list<UnitActions> unitActions;
  std::map<aw::unit::ptr, UnitGraphicsItem*> managedUnits;

  PathGraphicsItem* pathGraphicsItem;
};

#endif

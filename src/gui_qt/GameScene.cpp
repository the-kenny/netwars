#include "GameScene.h"

#include <QGraphicsView>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include "PixmapDrawing.h"
#include "Sprites.h"

#include "game/config.h"
#include "game/gui/drawing.h"
#include "game/gui/paths.h"

using namespace aw;

GameScene::GameScene(QWidget* parent) 
	: QGraphicsScene(parent), 
	  backgroundImage(":/data/background.png"),
	  pathGraphicsItem(new PathGraphicsItem) {

  this->setSceneRect(QRectF(0.0, 0.0, 16.0*30, 16.0*20));

  this->addItem(pathGraphicsItem);
}

void GameScene::setScene(aw::scene::ptr scene) { 
  processNewScene(scene);

  currentScene = scene;

  pathGraphicsItem->setScene(scene);
  update();

  BOOST_FOREACH(QGraphicsView* view, views())
	view->repaint();
}

void GameScene::reset() {
  currentScene.reset();

  //Reset the unit layers
  typedef std::pair<aw::unit::ptr, UnitGraphicsItem*> pair_t;
  BOOST_FOREACH(const pair_t& p, managedUnits) {
	removeItem(p.second);
	delete p.second;
  }

  managedUnits.clear();
  
  _signalClicked.disconnect_all_slots();
  _signalFocusChanged.disconnect_all_slots();
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    aw::coord realCoord(event->scenePos().x()/16, event->scenePos().y()/16);

	if(realCoord.x < 30 && realCoord.y < 20) {
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
}

void GameScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
	static coord lastCoord(0, 0);
	coord currentCoord(event->scenePos().x()/16, event->scenePos().y()/16);

	if(currentCoord.x < 30 && currentCoord.y < 20) {
	  if(currentCoord != lastCoord) {
		_signalFocusChanged(currentCoord);
		lastCoord = currentCoord;
	  }
	}
}

void GameScene::drawBackground(QPainter* painter, const QRectF& rect) {
  painter->drawImage(QPointF(0.0, 0.0), backgroundImage);

  if(currentScene) {
	for(int x = 0; x < 30; ++x) {
	  for(int y = 0; y < 20; ++y) {
		Drawing::drawPixmap(aw::gui::get_sprite_for(aw::coord(x, y), 
													currentScene),
							aw::coord(x, y), 
							*painter);
	  }
	}
  }
}

void GameScene::drawItems(QPainter *painter,
						  int numItems, QGraphicsItem *items[],
						  const QStyleOptionGraphicsItem options[],
						  QWidget *widget) {
  BOOST_FOREACH(const UnitActions& a, unitActions) {
	switch(a.action) {
	case UnitActions::MOVED:
	  getUnitGraphicsItem(a.unit)->moveTo(mapToSceneCoord(a.position));
	  break;

	  //The next two things are handled directly by processNewScene().
	  //We need them only if animations for adding and removing will be
	  //added in the future.
	  /*
	case UnitActions::ADDED:
	  addUnitForDrawing(a.unit, a.position);
	  break;
	case UnitActions::REMOVED:
	  removeUnitFromDrawing(a.unit);
	  break;
	  */
	default:;
	}
  }

  unitActions.clear();
  
  for (int i = 0; i < numItems; ++i) {
	// Draw the item
	painter->save();
	painter->setMatrix(items[i]->sceneMatrix(), true);
	items[i]->paint(painter, options+i, widget);
	painter->restore();
  }
}

/*
Doesn't work... shitty Qt-Drawing
void GameScene::drawForeground(QPainter* painter, const QRectF& rect) {
  if(currentScene) {
	static const std::string& pixmapdir = aw::config().get<std::string>("/config/dirs/pixmaps");

	BOOST_FOREACH(const coord& c, currentScene->get_highlighted_area())
	  this->drawPixmap(pixmapdir + "misc/range.png", c, *painter);

	BOOST_FOREACH(const coord& c, currentScene->get_path_area())
	  this->drawPixmap(pixmapdir + "misc/path.png", c, *painter);

	if(currentScene->highlight())
      this->drawPixmap(pixmapdir + "misc/highlight.png",
        currentScene->highlight(),
					   painter);
  }
}
*/

UnitGraphicsItem* GameScene::getUnitGraphicsItem(const aw::unit::ptr& u) {
  std::map<aw::unit::ptr, UnitGraphicsItem*>::iterator it = managedUnits.find(u);

  if(it->first)
	return it->second;
  else
	return NULL;
}

void GameScene::addUnitForDrawing(const aw::unit::ptr &u, const aw::coord& c) {
  UnitGraphicsItem* ugi(new UnitGraphicsItem);
  ugi->setUnit(u);
  ugi->setPos(mapToSceneCoord(c));

  this->addItem(ugi);

  managedUnits.insert(std::make_pair(u, ugi));
}

void GameScene::removeUnitFromDrawing(const aw::unit::ptr &u) {
  UnitGraphicsItem* ugi = getUnitGraphicsItem(u);

  this->removeItem(ugi);
  managedUnits.erase(u);

  delete ugi;
}

void GameScene::processNewScene(const scene::ptr& newScene) {
  if(newScene && currentScene) {
	std::map<aw::unit::ptr, aw::coord> oldUnits;
	std::map<aw::unit::ptr, aw::coord> newUnits;
	typedef std::map<aw::unit::ptr, aw::coord>::iterator iterator;

	//Cycle through the old scene and collect all units
	for(int x = 0; x < 30; ++x) {
	  for(int y = 0; y < 20; ++y) {
		const unit::ptr &oldUnit = currentScene->get_unit(x, y);
		const unit::ptr &newUnit = newScene->get_unit(x, y);
		
		if(oldUnit != NULL && !oldUnit->is_dummy()) {
		  oldUnits.insert(std::make_pair(oldUnit, coord(x, y)));
		}
		
		if(newUnit != NULL && !newUnit->is_dummy()) {
		  newUnits.insert(std::make_pair(newUnit, coord(x, y)));
		}
	  }
	}

	typedef std::pair<aw::unit::ptr, aw::coord> pair;
	BOOST_FOREACH(pair newPair, newUnits) {
	  iterator it = oldUnits.find(newPair.first);
			
	  //Cycle through the old units and search for the new, then add it to the move-list
	  if(it != oldUnits.end()) {
		//Add the unit to the movement-array when the position has changed
		if(newPair.second != it->second) {
		  //LOG("Unit moved from %i|%i to %i|%i", it->second.x, it->second.y, newPair.second.x, newPair.second.y);

		  UnitActions action;
		  action.action = UnitActions::MOVED;
		  action.oldPosition = it->second;
		  action.position = newPair.second;
		  action.unit = newPair.first;
		  unitActions.push_back(action);

		  //Hack: Building capture-status
		  //Update the currentTerrain of the unit
		  //[[self getAnimatableUnit:it->first] setCurrentTerrain:newScene->get_terrain(newPair.second.x, newPair.second.y)];  
		}
	  } else {
		//Unit isn't in the old scene - it is new.
		addUnitForDrawing(newPair.first, newPair.second);

		/*
		UnitActions action;
		action.position = it->second;
		action.action = UnitActions::ADDED;
		action.unit = newPair.first;
		unitActions.push_back(action);
		*/
	  }

	  BOOST_FOREACH(pair oldPair, oldUnits) {
		iterator it = newUnits.find(oldPair.first);
		
		//Unit isn't in the new scene - it is gone.
		if(it == newUnits.end()) {
		  removeUnitFromDrawing(oldPair.first);
		  oldUnits.erase(oldPair.first);
		}
	  }
	}
  } else if(newScene) {
	//Cycle through the old scene and collect 
	for(int x = 0; x < 30; ++x) {
	  for(int y = 0; y < 20; ++y) {
		const unit::ptr &unit = newScene->get_unit(x, y);
		
		if(unit != NULL && !unit->is_dummy()) {
		  addUnitForDrawing(unit, coord(x, y));
		}
	  }
	}
  }
}


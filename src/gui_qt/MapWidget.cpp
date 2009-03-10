#include "MapWidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <boost/foreach.hpp>

#include "PixmapDrawing.h"
#include "Sprites.h"

#include "game/config.h"
#include "game/gui/drawing.h"
#include "game/gui/paths.h"

using namespace aw;



MapWidget::MapWidget(QWidget* parent) 
	: QGraphicsScene(parent), 
	backgroundImage(":/data/background.png") {
}

void MapWidget::setScene(aw::scene::ptr scene) { 
  processNewScene(scene);

  currentScene = scene;
  
  this->update(); 
  this->update(); 
}

void MapWidget::reset() {
  currentScene.reset();
  
  _signalClicked.disconnect_all_slots();
  _signalFocusChanged.disconnect_all_slots();
}

void MapWidget::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    aw::coord realCoord(event->scenePos().x()/16, event->scenePos().y()/16);

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

void MapWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
	static coord lastCoord(0, 0);
	coord currentCoord(event->scenePos().x()/16, event->scenePos().y()/16);

	if(currentCoord != lastCoord) {
		_signalFocusChanged(currentCoord);
		lastCoord = currentCoord;
	}
}

/*
void MapWidget::drawUnits(QPainter& painter) {
	for(int x = 0; x < 30; ++x) {
		for(int y = 0; y < 20; ++y) {
			aw::unit::ptr u = currentScene->get_unit(x, y);

			if(u != NULL && !u->is_dummy()) {
				Drawing::drawPixmap(aw::gui::get_path(u->type(), u->color()), 
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
					Drawing::drawPixmap(mask, aw::coord(x, y), painter);

					painter.restore();
				}

				int life = u->get_hp();
				if(life < 10 && life > 0)
					Drawing::drawPixmap(gui::get_path(unit::LIVE, life), aw::coord(x, y), painter);

				if(u->low_ammo())
					Drawing::drawPixmap(gui::get_path(unit::LOW_AMMO), aw::coord(x, y), painter);

				if(u->low_fuel())
					Drawing::drawPixmap(gui::get_path(unit::LOW_FUEL), aw::coord(x, y), painter);

				if(u->is_hidden())
					Drawing::drawPixmap(gui::get_path(unit::HIDDEN), aw::coord(x, y), painter);

				if(u->is_transporter() && boost::dynamic_pointer_cast<transporter>(u)->loaded_units_count() > 0)
					Drawing::drawPixmap(gui::get_path(unit::LOADED), aw::coord(x, y), painter);

				if(u->can_capture())
				{
					const terrain::ptr &t(currentScene->get_terrain(x, y));

					if(t->is_building() && boost::dynamic_pointer_cast<building>(t)->capture_points() < 20)
					{
						Drawing::drawPixmap(gui::get_path(unit::CAPTURE), aw::coord(x, y), painter);
					}
				}
			}
		}
	}
}
*/

void MapWidget::drawBackground(QPainter* painter, const QRectF& rect) {
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

void MapWidget::drawItems(QPainter *painter,
						  int numItems, QGraphicsItem *items[],
						  const QStyleOptionGraphicsItem options[],
						  QWidget *widget) {
  BOOST_FOREACH(const UnitActions& a, unitActions) {
	switch(a.action) {
	case UnitActions::MOVED:
	  getUnitGraphicsItem(a.unit)->moveTo(mapToSceneCoord(a.position));
	  break;
	case UnitActions::ADDED:
	  addUnitForDrawing(a.unit, a.position);
	  break;
	case UnitActions::REMOVED:
	  removeUnitFromDrawing(a.unit);
	}
  }
  
  for (int i = 0; i < numItems; ++i) {
	// Draw the item
	painter->save();
	painter->setMatrix(items[i]->sceneMatrix(), true);
	items[i]->paint(painter, options+i, widget);
	painter->restore();
  }
}

UnitGraphicsItem* MapWidget::getUnitGraphicsItem(const aw::unit::ptr& u) {
  return managedUnits[u];
}

void MapWidget::addUnitForDrawing(const aw::unit::ptr &u, const aw::coord& c) {
  UnitGraphicsItem* ugi(new UnitGraphicsItem);
  ugi->setUnit(u);
  ugi->moveTo(mapToSceneCoord(c));

  this->addItem(ugi);

  managedUnits.insert(std::make_pair(u, ugi));
}

void MapWidget::removeUnitFromDrawing(const aw::unit::ptr &u) {
  UnitGraphicsItem* ugi = getUnitGraphicsItem(u);

  this->removeItem(ugi);
  managedUnits.erase(u);

  delete ugi;
}

void MapWidget::processNewScene(const scene::ptr& newScene) {
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

		UnitActions action;
		action.position = it->second;
		action.action = UnitActions::ADDED;
		action.unit = newPair.first;
		unitActions.push_back(action);
	  }

	  BOOST_FOREACH(pair oldPair, oldUnits) {
		iterator it = newUnits.find(oldPair.first);
		
		//Unit isn't in the new scene - it is gone.
		if(it == newUnits.end()) {
		  removeUnitFromDrawing(oldPair.first);
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


#ifndef AW_QT_PATH_GRAPHICS_ITEM_H
#define AW_QT_PATH_GRAPHICS_ITEM_H

#include <QGraphicsItem>
#include "PixmapDrawing.h"

#include "game/scene.h"
#include "game/config.h"

class PathGraphicsItem: public QGraphicsItem {
public:
  PathGraphicsItem() {
	//Draw it ontop of the stack
	this->setZValue(2);
  }
  
  aw::scene::ptr scene() const { return currentScene; }
  void setScene(const aw::scene::ptr& scene) { currentScene = scene; update(); }

protected:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* /* option */, QWidget* /* widget */) {

	if(currentScene) {
	  const static std::string& pixmapdir = aw::config().get<std::string>("/config/dirs/pixmaps");
	  
	  BOOST_FOREACH(const aw::coord& c, currentScene->get_highlighted_area())
		Drawing::drawPixmap(pixmapdir + "misc/range.png", c, *painter);
	  
	  BOOST_FOREACH(const aw::coord& c, currentScene->get_path_area())
		Drawing::drawPixmap(pixmapdir + "misc/path.png", c, *painter);
	  
	  if(currentScene->highlight())
		Drawing::drawPixmap(pixmapdir + "misc/highlight.png",
							currentScene->highlight(), 
							*painter);
	}
  }

  virtual QRectF boundingRect() const {
	return QRectF(0.0, 0.0, 16.0*30.0, 16.0*20.0);
  }
  
private:
  aw::scene::ptr currentScene;
};

#endif

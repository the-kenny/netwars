#ifndef AW_QT_UNLOAD_MENU_H
#define AW_QT_UNLOAD_MENU_H

#include <QDialog>
#include <QVBoxLayout>

#include <list>

#include "game/units/unit_base.h"

class UnitUnloadMenu: public QDialog {
  Q_OBJECT
	 
 public:
  UnitUnloadMenu(QWidget* parent);
  int run();

  static int showUnloadMenu(QWidget* parent, const std::list<aw::unit::ptr>& units);

 private Q_SLOTS:
  void setReturnValue(int value);

 private:
  void addAction(aw::unit::ptr u, int pos);

  QVBoxLayout* widgetLayout;
  int returnPosition;
};

#endif

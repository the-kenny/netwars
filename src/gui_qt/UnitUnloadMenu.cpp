#include "UnitUnloadMenu.h"

#include <QApplication>
#include <QPushButton>

#include "UnitUnloadMenuButton.h"

#include "game/units/unit_loader.h"

UnitUnloadMenu::UnitUnloadMenu(QWidget* parent)
  : QDialog(parent, Qt::Popup) {
  this->setWindowModality(Qt::ApplicationModal);
  
	widgetLayout = new QVBoxLayout;
	widgetLayout->setSpacing(0);
	widgetLayout->setContentsMargins(0, 0, 0, 0);

	this->setLayout(widgetLayout);

	this->move(QCursor::pos());

	returnPosition = -1;
}

int UnitUnloadMenu::run() {
  this->show();
  this->exec();

  return returnPosition;
}

void UnitUnloadMenu::setReturnValue(int pos) {
  returnPosition = pos;
}

int UnitUnloadMenu::showUnloadMenu(QWidget* parent, const std::list<aw::unit::ptr>& units) {
  UnitUnloadMenu m(parent);

  std::list<aw::unit::ptr>::const_iterator it = units.begin();
  int i = 0;
  for(; it != units.end(); ++it, ++i)
	 m.addAction(*it, i);

  return m.run();
}

void UnitUnloadMenu::addAction(aw::unit::ptr u, int pos) {
  UnloadButton* pushButton = new UnloadButton(this->parentWidget(), u, pos);
  pushButton->setText(aw::unit_list().get_unit_info(u->type()).name.c_str());
  //pushButton->setFlat(true);
  
  connect(pushButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(pushButton, SIGNAL(actionClicked(int)), this, SLOT(setReturnValue(int)));
  
  widgetLayout->addWidget(pushButton);
}

#include "UnitActionMenu.h"

#include <QApplication>
#include <QPushButton>

#include "UnitActionMenuButton.h"

#include <boost/foreach.hpp>

#include "game/gui/unit_action_menu.h"

#include <iostream>

UnitActionMenu::UnitActionMenu(QWidget* parent)
: QDialog(parent, Qt::Popup) {
	this->setWindowModality(Qt::ApplicationModal);

	widgetLayout = new QVBoxLayout;
	widgetLayout->setSpacing(0);
	widgetLayout->setContentsMargins(0, 0, 0, 0);

	this->setLayout(widgetLayout);

	this->move(QCursor::pos());

	_returnValue = aw::units::WAIT;
}

aw::units::actions UnitActionMenu::run() {
   this->addAction(aw::units::CANCEL);
	this->show();
	this->exec();

	return _returnValue;
}

aw::units::actions UnitActionMenu::showActionMenu(QWidget* parent, const std::list<aw::units::actions>& actions) {
	UnitActionMenu m(parent);
	BOOST_FOREACH(aw::units::actions a, actions)
		m.addAction(a);
	
	return m.run();
}

void UnitActionMenu::setReturnValue(aw::units::actions a) {
	_returnValue = a;
}

void UnitActionMenu::addAction(aw::units::actions action) {
	ActionButton* pushButton = new ActionButton(this->parentWidget(), action);
	pushButton->setText(aw::gui::unit_action_menu::get_name(action).c_str());
	//pushButton->setFlat(true);

	connect(pushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(pushButton, SIGNAL(actionClicked(aw::units::actions)), this, SLOT(setReturnValue(aw::units::actions)));

	widgetLayout->addWidget(pushButton);
}

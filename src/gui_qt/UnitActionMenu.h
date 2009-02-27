#ifndef AW_QT_UNIT_ACTION_MENU_H
	#define AW_QT_UNIT_ACTION_MENU_H

#include <QPushButton>
#include <QVBoxLayout>
#include <QDialog>

#include "game/units/actions.h"

#include <list>

class UnitActionMenu: public QDialog {
	Q_OBJECT

	public:
		UnitActionMenu(QWidget* parent);

		aw::units::actions run();
		static aw::units::actions showActionMenu(QWidget* parent, const std::list<aw::units::actions>& actions);
	
	private Q_SLOTS:
		void setReturnValue(aw::units::actions a);

	private:
		//A custom button class with a new signal...

		void addAction(aw::units::actions action);
		
		QVBoxLayout* widgetLayout;
		aw::units::actions _returnValue;
};

#endif

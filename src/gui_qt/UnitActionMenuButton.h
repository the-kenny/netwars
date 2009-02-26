#ifndef AW_QT_UNIT_ACTION_MENU_BUTTON_H
	#define AW_QT_UNIT_ACTION_MENU_BUTTON_H

#include <QPushButton>

#include "game/units/actions.h"

//A class for a button which holds data and emits a signal where the data is an argument
class ActionButton: public QPushButton {
	Q_OBJECT

	public:
		ActionButton(QWidget* parent, aw::units::actions action)
			: QPushButton(parent) {
				this->action = action;

				connect(this, SIGNAL(clicked()), this, SLOT(emitSignal()));
			}

	Q_SIGNALS:
		void actionClicked(aw::units::actions);
	private Q_SLOTS:
		void emitSignal() {
			Q_EMIT actionClicked(action);
		}
	
	private:
		aw::units::actions action;
};

#endif

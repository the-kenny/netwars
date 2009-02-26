#ifndef AW_QT_UNIT_UNLOAD_MENU_BUTTON_H
#define AW_QT_UNIT_UNLOAD_MENU_BUTTON_H

#include <QPushButton>

#include "Sprites.h"
#include "game/units/unit_base.h"
#include "game/gui/paths.h"

//A class for a button which holds data and emits a signal where the data is an argument
class UnloadButton: public QPushButton {
	Q_OBJECT

	public:
  UnloadButton(QWidget* parent, aw::unit::ptr unit, int pos)
			: QPushButton(parent) {
				position = pos;

				this->setIcon(QPixmap::fromImage(sharedSprites().getSprite(aw::gui::get_path(unit->type(), unit->color()))));

				connect(this, SIGNAL(clicked()), this, SLOT(emitSignal()));
			}

	Q_SIGNALS:
		void actionClicked(int position);
	private Q_SLOTS:
		void emitSignal() {
			Q_EMIT actionClicked(position);
		}
	
	private:
		int position;
};

#endif

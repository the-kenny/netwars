#ifndef AW_QT_BUY_MENU_H
	#define AW_QT_BUY_MENU_H

#include <QDialog>
#include <QStandardItemModel>
#include <QTreeWidgetItem>
#include <QWidget>

#include "ui_buyMenu.h"

#include "game/player.h"
#include "game/units/units.h"

class BuyMenu: public QDialog, public Ui::BuyMenu {
	Q_OBJECT

	public:
		BuyMenu(QWidget* parent, aw::player::ptr player, aw::unit::workshops workshop);

		static aw::unit::types showBuyMenu(QWidget* parent, aw::player::ptr player, aw::unit::workshops workshop);

	private Q_SLOTS:
		void unitSelected(QTreeWidgetItem*, QTreeWidgetItem*);

	private:
		int playerFunds;
		aw::unit::types _returnValue;
};

#endif

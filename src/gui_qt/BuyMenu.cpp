#include "BuyMenu.h"

#include <QBrush>
#include <QPushButton>
#include <QStringList>

#include <boost/foreach.hpp>

#include "Sprites.h"

#include "game/units/unit_loader.h"
#include "game/gui/paths.h"

using namespace aw;

BuyMenu::BuyMenu(QWidget* parent, aw::player::ptr player, aw::unit::workshops workshop)
	: QDialog(parent) {
		setupUi(this);
		treeWidget->setColumnWidth(0, 20);

		playerFunds = player->get_funds();
		startFundsLabel->setText(QString("%1").arg(playerFunds));
		priceLabel->setText(QString("0"));
		differenceLabel->setText(QString("%1").arg(playerFunds));

		connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), 
				this, SLOT(unitSelected(QTreeWidgetItem*, QTreeWidgetItem*)));


		BOOST_FOREACH(const aw::unit_loader::value_type &pair, aw::unit_list()) {
			const aw::unit_loader::loaded_unit& loaded_unit = pair.second;

			if(!(loaded_unit.unit->is_dummy()) && loaded_unit.produced_in == workshop) {
				QStringList items;
				items.push_back("   ");
				items.push_back(loaded_unit.name.c_str());
				items.push_back(QString("%1").arg(loaded_unit.price));
				items.push_back(loaded_unit.internal_name.c_str());

				QTreeWidgetItem* treeItem = new QTreeWidgetItem(items);

				QImage image = sharedSprites().getSprite(gui::get_path(loaded_unit.unit->type(), 
							player->get_unit_color()).c_str());
				treeItem->setIcon(0, QIcon(QPixmap::fromImage(image)));

				if(loaded_unit.price > playerFunds) {
					treeItem->setDisabled(true);
				}

				treeWidget->addTopLevelItem(treeItem);
			}
		}
	}


aw::unit::types BuyMenu::showBuyMenu(QWidget* parent, aw::player::ptr player, aw::unit::workshops workshop) {
	BuyMenu b(parent, player, workshop);
	if(b.exec() == QDialog::Accepted)
	  return b._returnValue;
	else
	  return aw::unit::TYPE_NONE;
}

void BuyMenu::unitSelected(QTreeWidgetItem* current, QTreeWidgetItem* /* last */) {
	QString colorTemplate = tr("<font color='%1'>%2</font>");
	
	int priceDifference = playerFunds - current->text(2).toInt();

	QString color = "black";
	if(priceDifference < 0)
		color = "red";

	priceLabel->setText(current->text(2));
	differenceLabel->setText(colorTemplate.arg(color).arg(priceDifference));

	//Deactivate the OK-Button if we can't afford the unit
	buttonBox->button(QDialogButtonBox::Ok)->setDisabled(current->isDisabled());

	_returnValue = current->text(3).toStdString();
}

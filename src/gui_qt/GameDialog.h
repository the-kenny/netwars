#ifndef AW_QT_GAME_DIALOG_H
	#define AW_QT_GAME_DIALOG_H

#include <QDialog>
#include <string>

#include "ui_gameDialog.h"

class GameDialog: public QDialog, public Ui::GameDialog {
	Q_OBJECT

	public:
		GameDialog(QDialog* parent = 0);

		int fundsPerBuilding() const { return _fundsPerBuilding; }
		int initialFunds() const { return _initialFunds; }
		const std::string& mapFile() const { return _mapFile; }

	private Q_SLOTS:
		void fundsPerBuildingChanged(int newValue);
		void initialFundsChanged(int newValue);
		void mapFileChanged(QString newValue);

		void chooseMapFile();
	
	private:
		int _fundsPerBuilding;
		int _initialFunds;
		std::string _mapFile;
};

#endif

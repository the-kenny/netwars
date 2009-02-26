#include "GameDialog.h"

#include <QFileDialog>


GameDialog::GameDialog(QDialog* parent) {
	setupUi(this);

	//Connect Slots
	connect(initialFundsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(initialFundsChanged(int)));
	connect(fundsPerBuildingSpinBox, SIGNAL(valueChanged(int)), this, SLOT(fundsPerBuildingChanged(int)));
	connect(chooseMapButton, SIGNAL(clicked()), this, SLOT(chooseMapFile()));
	connect(mapFileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(mapFileChanged(QString)));

	_initialFunds = 0;
	_fundsPerBuilding = 1000;
}


void GameDialog::fundsPerBuildingChanged(int newValue) {
	_fundsPerBuilding = newValue;
}

void GameDialog::initialFundsChanged(int newValue) {
	_initialFunds = newValue;
}

void GameDialog::mapFileChanged(QString newValue) {
	mapFileLineEdit->setText(newValue);
	_mapFile = newValue.toStdString();
}


void GameDialog::chooseMapFile() {
	static const QString mapEditorFilter = "MapEditor-Files (*.aws *.awd *.aw2 *.awm)";
	this->mapFileChanged(QFileDialog::getOpenFileName(this, "Choose a Map", QString(), mapEditorFilter));
}


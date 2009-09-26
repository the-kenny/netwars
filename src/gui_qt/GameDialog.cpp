#include "GameDialog.h"

#include <QFileDialog>

#include "game/config.h"


GameDialog::GameDialog(QDialog* parent) {
	setupUi(this);

	//Connect Slots
	connect(initialFundsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(initialFundsChanged(int)));
	connect(fundsPerBuildingSpinBox, SIGNAL(valueChanged(int)), this, SLOT(fundsPerBuildingChanged(int)));
	connect(chooseMapButton, SIGNAL(clicked()), this, SLOT(chooseMapFile()));
	connect(mapFileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(mapFileChanged(QString)));



	_initialFunds = aw::config().get<int>("/config/defaults/initial-funds");
	_fundsPerBuilding = aw::config().get<int>("/config/defaults/funds-per-building");

	//Show the values in the spinBoxes
	initialFundsSpinBox->setValue(_initialFunds);
	fundsPerBuildingSpinBox->setValue(_fundsPerBuilding);
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

	if(minimapThread != NULL) {
	  minimapThread->terminate();
	  disconnect(minimapThread, SIGNAL(minimapReady(QImage)), this, SLOT(setMinimapImage(QImage)));
	  minimapThread = NULL;
	}

	minimapThread = new MinimapDrawingThread(_mapFile,
											 imageLabel);

	//Minimap
	connect(minimapThread, SIGNAL(minimapReady(QImage)), this, SLOT(setMinimapImage(QImage)), Qt::QueuedConnection);

	minimapThread->start();
}


void GameDialog::chooseMapFile() {
	static const QString mapEditorFilter = "MapEditor-Files (*.aws *.awd *.aw2 *.awm)";
	this->mapFileChanged(QFileDialog::getOpenFileName(this, "Choose a Map", QString(), mapEditorFilter));
}

void GameDialog::setMinimapImage(QImage image) {
  imageLabel->setPixmap(QPixmap::fromImage(image));
}

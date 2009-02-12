#include "MainWindow.h"

#include <iostream>

MainWindow::MainWindow(QMainWindow* parent) {
	setupUi(this);

	//Connect slots
	connect(actionNewGame, SIGNAL(triggered()), this, SLOT(newGame()));
}


void MainWindow::newGame() {
	std::cout << "newGame" << std::endl;
}

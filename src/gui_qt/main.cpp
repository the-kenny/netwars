#include <QApplication>

#include "MainWindow.h"

#include "game/config.h"

int main(int argc, char *argv[]) { 
	QApplication app(argc, argv); 

	aw::config().load("config.xml");

	MainWindow w;
	w.show();

	return app.exec(); 
}

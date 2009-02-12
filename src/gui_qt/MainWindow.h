#ifndef AW_QT_MAIN_WINDOW_H
	#define AW_QT_MAIN_WINDOW_

#include "ui_mainWindow.h"

class MainWindow: public QMainWindow, public Ui::MainWindow {
	Q_OBJECT

	public:
		MainWindow(QMainWindow* parent = 0);
};

#endif

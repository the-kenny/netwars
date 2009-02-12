#ifndef AW_QT_MAIN_WINDOW_H
	#define AW_QT_MAIN_WINDOW_

#include "ui_mainWindow.h"

#include "game/game_controller.h"

class MainWindow: public QMainWindow, public Ui::MainWindow {
	Q_OBJECT

	public:
		MainWindow(QMainWindow* parent = 0);

	public Q_SLOTS:
		void newGame();

	private:
		aw::game_controller::ptr gameController;
};

#endif

#ifndef AW_QT_MAIN_WINDOW_H
	#define AW_QT_MAIN_WINDOW_

#include "ui_mainWindow.h"

#include "GameScene.h"

#include "game/game_controller.h"

class MainWindow: public QMainWindow, public Ui::MainWindow {
	Q_OBJECT

	public:
		MainWindow(QMainWindow* parent = 0);

	public Q_SLOTS:
		void newGame();
		void endTurn();

	private:
		aw::game_controller::ptr gameController;
  GameScene* graphicsScene;

  void gameFinished(const aw::player::ptr& p);
};

#endif

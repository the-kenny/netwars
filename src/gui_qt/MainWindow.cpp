#include "MainWindow.h"

#include "game/game.h"
#include "game/game_controller.h"

#include <boost/bind.hpp>

using namespace aw;

MainWindow::MainWindow(QMainWindow* parent) {
	setupUi(this);

	//Connect slots
	connect(actionNewGame, SIGNAL(triggered()), this, SLOT(newGame()));
}


void MainWindow::newGame() {
	game::ptr game(new aw::game());

	game->set_funds_per_building(1000);
	game->load_map("7330.aws");

	gameController = game_controller::ptr(new game_controller);

	drawingArea->signalClicked().connect(boost::bind(&aw::game_controller::click, gameController, _1, _2));
	drawingArea->signalFocusChanged().connect(boost::bind(&aw::game_controller::mouse_hover_changed, gameController, _1));

	gameController->signal_scene_change().connect(boost::bind(&MapWidget::setScene, drawingArea, _1));


	gameController->start_game(game);
}

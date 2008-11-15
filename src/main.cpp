#include <gtkmm.h>

#include "application.h"

#include <cstdlib>
#include <ctime>

int main(int argc, char **argv)
{
	aw::application app;
	return app.run(argc, argv);
}



#include <gtkmm.h>
 
#include "gui/gtk/main_window.h"
 
#include <cstdlib>
#include <ctime>
 /*
int main(int argc, char **argv)
{
  std::srand(std::time(NULL));
 
  try
  {
    Gtk::Main kit(argc, argv);
    aw::gui::gtk::main_window w;
 
    Gtk::Main::run(w);
  }
  catch(const std::exception &e)
  {
    std::cerr << "Uncaught Exception: " << e.what() << std::endl;
    std::cin.get();
  }
}
 
 */
#ifndef AW_GTK_GAME_DIALOG_H
	#define AW_GTK_GAME_DIALOG_H

#include <gtkmm.h>

#include "game/gui/game_dialog.h"

namespace aw
{
	namespace gui
	{
		//TODO(#1): Verschiedene Spielerreihenfolgen ermöglichen
		//TODO: Use Glade::VariableMap instead of all the callbacks
		//TODO(#5): game_settings Klasse nutzen
		namespace gtk
		{
			class new_game_dialog: public Gtk::Window, public aw::gui::game_dialog
			{
				public:
					new_game_dialog();

					virtual response_t run();

					Gtk::VBox *m_root_vbox;

				private:
					void on_start_button_clicked();
					void on_cancel_button_clicked();
					void initial_spin_toggled(Gtk::SpinButton *spin);
					void fpb_spin_toggled(Gtk::SpinButton *spin);
					void on_map_select_button_clicked(Gtk::Entry *l);

					std::string show_map_selection();

					bool m_return;
					response_t m_response;
			};
		}
	}
}

#endif

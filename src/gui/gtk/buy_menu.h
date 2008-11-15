#ifndef AW_DISPLAY_GTK_BUY_MENU_H
	#define AW_DISPLAY_GTK_BUY_MENU_H

#include <gtkmm.h>
#include <boost/signals.hpp>
#include <libglademm/xml.h>

#include "gui/buy_menu.h"

#include "game/units/unit_base.h"
#include "game/player.h"



namespace aw
{
	namespace gui
	{
		namespace gtk
		{
			class unit_view: public Gtk::TreeView
			{
				public:
					typedef boost::signal<void (Gtk::TreeModel::Row)> signal_selection_changed_t;

					unit_view(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
					: Gtk::TreeView(cobject)
					{

					}

					signal_selection_changed_t &on_selection_changed() { return m_signal_selection_changed; }

				protected:
					virtual bool on_button_press_event(GdkEventButton *event);

				private:
					signal_selection_changed_t m_signal_selection_changed;
			};

			class buy_menu: public Gtk::Window, public aw::gui::buy_menu
			{
				public:
					class model_columns: public Gtk::TreeModelColumnRecord
					{
						public:
							model_columns();

							Gtk::TreeModelColumn<bool> affordable;
							Gtk::TreeModelColumn<std::string> name;
							Gtk::TreeModelColumn<int> price;
							Gtk::TreeModelColumn<unit::types> type;
							Gtk::TreeModelColumn<unit::colors> color;
					};

					buy_menu(unit::workshops shop, const player::ptr &player);
					virtual unit::types run();

				private:
					void row_click_callback(Gtk::TreeModel::Row r);
					bool select_callback(const Glib::RefPtr<Gtk::TreeModel>& model, const Gtk::TreeModel::Path& path, bool);
					virtual void buy(unit::types t);
					void cancel();
					
					void refresh();

					Gtk::Label *m_funds_label;

					sigc::connection m_unit_connection;

					model_columns m_columns;

					Glib::RefPtr<Gtk::ListStore> m_list;
					unit_view *m_unit_view;

					Gtk::Button *m_buy_button;

					volatile bool m_return;
			};

		}
	}
}

#endif

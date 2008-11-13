#ifndef AW_GAME_DIALOG_H
	#define AW_GAME_DIALOG_H

#include <gtkmm.h>

#include "constants.h"

namespace aw
{
	namespace gui
	{
		//TODO(#1): Verschiedene Spielerreihenfolgen ermöglichen
		//TODO: Use Glade::VariableMap instead of all the callbacks
		//TODO(#5): game_settings Klasse nutzen
		class game_dialog
		{
			public:
//				const bool *players() { return m_players; }
				int funds_per_building() { return m_funds_per_building; }
				int initial_funds() { return m_initial_funds; }
				const std::string &map() { return m_map; }

				virtual response_t run() = 0;

//				bool run();

			protected:
//				bool m_players[4];
				int m_funds_per_building;
				int m_initial_funds;
				std::string m_map;
		};
	}
}

#endif

#include "game_settings.h"
#include "map_loader/map_loader.h"
#include "game_mechanics/player_utilities.h"

using namespace aw;

void game_settings::load_map(const std::string &filename)
{
	m_map_filename = filename;

	map_loader m;
	map_loader::loaded_map::ptr loaded_map = m.load(filename);

	m_map = map::ptr(new map(loaded_map));

	if(game_mechanics::participates(m_map, player::RED))
	{
		player::ptr p(new player(player::RED));
		m_players.push_back(p);
	}

	if(game_mechanics::participates(m_map, player::BLUE))
	{
		player::ptr p(new player(player::BLUE));
		m_players.push_back(p);
	}

	if(game_mechanics::participates(m_map, player::GREEN))
	{
		player::ptr p(new player(player::GREEN));
		m_players.push_back(p);
	}

	if(game_mechanics::participates(m_map, player::YELLOW))
	{
		player::ptr p(new player(player::YELLOW));
		m_players.push_back(p);
	}

	if(game_mechanics::participates(m_map, player::BLACK))
	{
		player::ptr p(new player(player::BLACK));
		m_players.push_back(p);
	}

	m_players.set_first_player(0);
}

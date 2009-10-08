#ifndef AW_PLAYER_H
#define AW_PLAYER_H

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "units/units.h"
#include "terrain.h"

#include <map>
#include <stdexcept>

namespace aw
{
  class player
  {
  public:
	typedef boost::shared_ptr<player> ptr;
	enum colors { RED, BLUE, GREEN, YELLOW, BLACK };

	player(colors c);

	colors get_color() const { 
	  return m_color; 
	}

	int get_funds() const { 
	  return m_funds; 
	}
	
	void set_funds(unsigned int funds) { 
	  m_funds = funds; 
	}

	void subtract_funds(unsigned int funds) {
	  if(static_cast<int>(m_funds - funds) < 0) 
		throw std::runtime_error("Not enough funds");
	  m_funds -= funds;
	}

	void add_funds(unsigned int funds) {
	  m_funds += funds;
	}

	unit::colors get_unit_color() const;
	bool his_unit(const unit::ptr &u) const {
	  return (u->color() == this->get_unit_color());
	}

	terrain::extras get_building_color() const;
	bool his_building(const terrain::ptr &p) const {
	  assert(p->is_building());
	  return (p->extra() == this->get_building_color());
	}

	void start_turn();
	void end_turn();

	bool has_units() const { 
	  return m_has_units; 
	}
	void has_units(bool u) { 
	  m_has_units = u; 
	}

	std::string get_color_string() const;
	void print(std::ostream &o) const;

  private:
	int m_funds; //should not be negative
	//				int m_funds_per_turn;
	const colors m_color;

	bool m_has_units;
  };

  class player_manager
  {
  public:
	typedef std::vector<player::ptr> container_t;

	void add(const player::ptr &p) { 
	  m_players.push_back(p); 
	}

	void erase(const player::ptr &p) {
	  container_t::iterator it = std::find(m_players.begin(),
										   m_players.end(), 
										   p);
	  assert(it != m_players.end());
	  m_players.erase(it);
	}

	void set_first_player(std::size_t index) {
	  m_active_player = m_players.begin()+index;
	}

	void set_first_player(const player::ptr& p) {
	  container_t::iterator it = std::find(m_players.begin(), 
										   m_players.end(),
										   p);
	  assert(it != m_players.end());
	  m_active_player = it;
	}

	player::ptr &get_active_player() {
	  return *m_active_player;
	}

	void next() {
	  if(m_active_player+1 != m_players.end())
		m_active_player++;
	  else
		m_active_player = m_players.begin();
	}

	player::ptr at(std::size_t index) {
	  return m_players[index];
	}

	container_t &get_players() { 
	  return m_players; 
	}
	
	const container_t &get_players() const { 
	  return m_players; 
	}

  private:
	container_t m_players;
	container_t::iterator m_active_player;
  };

  std::ostream &operator<<(std::ostream &o, const player &p);
}

#endif

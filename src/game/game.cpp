#include "game.h"

#include "game_mechanics/explosion_damage.h"
#include "game_mechanics/damage_calc.h"
#include "game_mechanics/prices.h"
#include "game_mechanics/player_utilities.h"
#include "game_mechanics/supply_mechanics.h"

#include "map_loader/map_loader.h"

#include <stdexcept>

using namespace aw;

void game::begin_unit_move(const coord &from, const coord &to)
{
	assert(m_active_move.active == false);

	m_active_move.active = true;
	m_active_move.done = false;

	//Move dummy-unit aside
	if(this->get_unit(to) && this->get_unit(to)->is_dummy())
	{
		m_removed_dummy_units.insert(std::make_pair(to, this->get_map()->detach_unit(to)));
	}

	m_map->move_unit(from, to);

	m_active_move.from = from;
	m_active_move.to = to;
}

void game::complete_unit_move(const game_mechanics::path &path)
{
	assert(m_active_move.active == true);
	assert(m_active_move.done == false);

	const coord &from = m_active_move.from;
	const coord &to = m_active_move.to;

	assert(path.start() == from);
	assert(path.end() == to);

	unit::ptr unit = m_map->get_unit(to);
	assert(unit != NULL);
	assert(unit->moved() == false);
	unit->move(path.get_fuel_costs());

	const terrain::ptr terr = m_map->get_terrain(from);
	if(terr->is_building())
	{
		building::ptr b = boost::dynamic_pointer_cast<building>(terr);
		b->reset_capture_points();
	}

	//Place dummy-units back
	std::map<coord, dummy_unit::ptr>::iterator it = m_removed_dummy_units.find(from);
	if(it != m_removed_dummy_units.end())
	{
		this->get_map()->add_unit(from, it->second);
	}

	m_active_move.reset();
}

void game::cancel_unit_move()
{
	assert(m_active_move.active == true);

	m_map->move_unit(m_active_move.to, m_active_move.from);

	//Place dummy-units back
	std::map<coord, dummy_unit::ptr>::iterator it = m_removed_dummy_units.find(m_active_move.to);
	if(it != m_removed_dummy_units.end())
	{
		this->get_map()->add_unit(m_active_move.to, it->second);
		m_removed_dummy_units.erase(it);
	}

	m_active_move.reset();
}


void game::attack_unit(const coord &attacker_c, const coord &victim_c)
{
	const unit::ptr attacker = m_map->get_unit(attacker_c);
	const unit::ptr victim = m_map->get_unit(victim_c);

	assert(attacker != NULL);
	assert(victim != NULL);
	assert(attacker->has_shot() == false);

	game_mechanics::damage_calc calc;
	calc.calculate(m_map, attacker_c, victim_c);

	//When the victim is a dummy, there are necessary steps for the manipulation of the terrain etc.
	//TODO: Dummy-Units gets unfloored/unceiled damage
	if(victim->is_dummy())
	{
		float damage = calc.get_victim_damage();

		if(victim->get_hp() - damage*10 <= 0)
		{
			m_map->delete_unit(victim_c);
			m_map->get_terrain(victim_c)->type(terrain::WRECKAGE);
		}
		else
		{
			victim->set_hp(victim->get_hp_as_float() - damage*10);
		}
	}
	else
	{
		if(calc.get_unit1_life() <= 0)
			m_map->delete_unit(attacker_c);
		else
			attacker->apply_damage(calc.get_attacker_damage());

		if(calc.get_unit2_life() <= 0)
			m_map->delete_unit(victim_c);
		else
			victim->apply_damage(calc.get_victim_damage());
	}

	this->check_for_defeat();
}

void game::explode(const coord &unit_c)
{
	unit::ptr bomb = m_map->get_unit(unit_c);
	assert(bomb != NULL);
	assert(bomb->can_explode() == true);

	game_mechanics::explosion_damage damage;
	damage.calculate(m_map, unit_c, bomb->get_explosion_range(), bomb->get_explosion_damage());

	BOOST_FOREACH(const game_mechanics::explosion_damage::value_type &p, damage)
	{
		const coord &c = p.first;
		if(m_map->get_unit(c))
		{
			m_map->get_unit(c)->set_hp_as_float(p.second);
		}
	}

	m_map->delete_unit(unit_c);

	this->check_for_defeat();
}

void game::knock_down(const coord &att_c, const coord &target_c)
{
	unit::ptr att = m_map->get_unit(att_c);
	unit::ptr vic = m_map->get_unit(target_c);

	assert(att != NULL);
	assert(vic != NULL);
	assert(att->can_run_over() == true);

	m_map->delete_unit(target_c);

	this->check_for_defeat();
}

void game::launch_silo(const coord &silo_c, const coord &target_c)
{
	silo::ptr s = boost::dynamic_pointer_cast<silo>(m_map->get_terrain(silo_c));

	assert(s != NULL);
	assert(!s->has_fired());

	game_mechanics::explosion_damage damage;
	damage.calculate(m_map, target_c, s->get_range(), s->get_damage());

	BOOST_FOREACH(const game_mechanics::explosion_damage::value_type &p, damage)
	{
		const coord &c = p.first;
		if(m_map->get_unit(c))
		{
			m_map->get_unit(c)->set_hp_as_float(p.second);
		}
	}

	s->fire();

	this->check_for_defeat();
}

void game::capture_building(const coord &unit_c)
{
	const unit::ptr unit = m_map->get_unit(unit_c);
	const terrain::ptr terr = m_map->get_terrain(unit_c);

	assert(unit != NULL);
	assert(unit->can_capture() == true);

	assert(terr->is_building() == true);

	building::ptr b = boost::dynamic_pointer_cast<building>(terr);

	if(b->capture() <= 0)
	{
		if(b->type() == terrain::HEADQUARTER && (b->extra() != terrain::WHITE && b->extra() != terrain::LAUNCHED))
			b->type(terrain::CITY);

		terrain::extras current_color = m_players.get_active_player()->get_building_color();
		b->extra(current_color);
		b->reset_capture_points();
	}

	this->check_for_defeat();
}

void game::supply_units(const coord &support_unit)
{
	const unit::ptr unit = this->get_unit(support_unit);
	assert(unit != NULL);
	assert(unit->can_supply() == true);

	area a = game_mechanics::get_supply_coordinates(this->get_map(), support_unit);

	BOOST_FOREACH(const coord &c, a)
	{
		if(this->on_map(c))
		{
			unit::ptr u = this->get_unit(c);
			if(u && game_mechanics::can_supply(this->get_map(), support_unit, c))
				u->supply();
		}
	}
}

void game::supply_unit_from_building(const coord &support_building)
{
	const building::ptr b = boost::dynamic_pointer_cast<building>(this->get_terrain(support_building));
	assert(b != NULL);
	assert(b->can_supply() == true);

	unit::ptr unit = this->get_unit(support_building);
	assert(unit != NULL);

	//I'm not sure if supplying (fuel and ammo) costs
	unit->supply();

	if(unit->get_hp() < unit->max_hp())
	{
		assert(b->get_supply_environment() == unit->get_environment());

		int repair_points = b->get_repair_points();
		if(repair_points != 0)
		{
			if(unit->get_hp()+repair_points >= unit->max_hp())
				unit->set_hp(unit->max_hp());
			else
				unit->set_hp(unit->get_hp()+repair_points);

			this->get_active_player()->subtract_funds(unit_loader::instance().get_unit_info(unit->get_name()).price/10);
		}
	}
}

void game::repair_unit(const coord &repair_unit, const coord &to)
{
	const unit::ptr unit = this->get_unit(repair_unit);
	assert(unit != NULL);
	assert(unit->can_repair() == true);

	unit::ptr rep_unit = this->get_unit(to);
	assert(rep_unit != NULL);

	assert(game_mechanics::can_repair(this->get_map(), repair_unit, to, this->get_active_player()->get_funds()) == true);

	rep_unit->supply();
	rep_unit->repair(unit->get_repair_points());

	this->get_active_player()->subtract_funds(unit_loader::instance().get_unit_info(rep_unit->get_name()).price/10);
}

//Takes care of the unit-movement
void game::load_unit(const coord &transporter_c, const coord &load_c, const game_mechanics::path &path)
{
	transporter::ptr t = boost::dynamic_pointer_cast<transporter>(m_map->get_unit(transporter_c));
	unit::ptr load = m_map->get_unit(load_c);

	assert(t != NULL);
	assert(load != NULL);

	assert(t->can_load(load->type()) == true);

//	assert(m_active_move.active == true);
//	assert(m_active_move.done == false);

	assert(load->moved() == false);
	load->move(path.get_fuel_costs()-1);

	t->load_unit(m_map->detach_unit(load_c));
}

void game::unload_unit(const coord &transporter_c, const game_mechanics::path &path, const coord &to, std::size_t index)
{
	assert(m_map->get_unit(to) == NULL);

	transporter::ptr t = boost::dynamic_pointer_cast<transporter>(m_map->get_unit(transporter_c));
	const terrain::ptr terr = m_map->get_terrain(to);

	assert(t != NULL);
	assert(t->can_unload() == true);
	assert(terr->can_pass(t->get_unit(index)->move_type()) == true);

	unit::ptr u = t->unload(index);
	assert(m_map->get_terrain(to)->can_pass(u->move_type()));


	m_map->add_unit(to, u);
	u->set_moved();

	if(m_active_move.active)
		this->complete_unit_move(path);
	else
		t->set_moved();
}


void game::hide_unit(const coord &unit_c)
{
	unit::ptr unit = m_map->get_unit(unit_c);
	assert(unit != NULL);
	assert(unit->can_hide() == true);

	unit->hide();
}

void game::appear_unit(const coord &unit_c)
{
	unit::ptr unit = m_map->get_unit(unit_c);
	assert(unit != NULL);
	assert(unit->can_hide() == true);

	unit->appear();
}

void game::buy_unit(const coord &pos, unit::types type)
{
	int price = game_mechanics::price<unit>(type);
	int funds = m_players.get_active_player()->get_funds();

	assert(!m_map->get_unit(pos));
	assert(price <= funds);

	this->get_active_player()->subtract_funds(price);
	m_map->add_unit(pos, aw::units::create(type, this->get_active_player()->get_unit_color()));
	m_map->get_unit(pos)->set_moved(true);


	BOOST_FOREACH(const player::ptr &p, m_players.get_players())
	{
		if(p->his_unit(m_map->get_unit(pos)))
		{
			p->has_units(true);
			break;
		}
	}
}

void game::join_units(const coord &from, const coord &to, const game_mechanics::path &path)
{
	//Unit1 joins into unit2
	unit::ptr unit1 = this->get_unit(from);
	unit::ptr unit2 = this->get_unit(to);

	assert(unit1 != NULL);
	assert(unit2 != NULL);

	assert(!unit1->moved());
	assert(unit1->type() == unit2->type());

	unit1->move(path.get_fuel_costs());
	unit2->join(*unit2);

	m_map->delete_unit(from);
}

void game::load_map(const std::string &file)
{
	aw::map_loader map_loader(file);
	aw::map_loader::loaded_map::ptr loaded_map = map_loader.load();

	aw::map::ptr map(new aw::map(loaded_map->m_terrain, loaded_map->m_unit));

	if(!map->valid())
		throw std::runtime_error("[game::load_map] " + file + " isn't valid.");

	this->set_map(map);
}

void game::start_game()
{
	bool participates_red = game_mechanics::participates(m_map, player::RED);
	bool participates_blue = game_mechanics::participates(m_map, player::BLUE);
	bool participates_green = game_mechanics::participates(m_map, player::GREEN);
	bool participates_yellow = game_mechanics::participates(m_map, player::YELLOW);
	bool participates_black = game_mechanics::participates(m_map, player::BLACK);

	if(participates_red)
	{
		player::ptr p(new player(player::RED));
		m_players.push_back(p);
	}

	if(participates_blue)
	{
		player::ptr p(new player(player::BLUE));
		m_players.push_back(p);
	}

	if(participates_green)
	{
		player::ptr p(new player(player::GREEN));
		m_players.push_back(p);
	}

	if(participates_yellow)
	{
		player::ptr p(new player(player::YELLOW));
		m_players.push_back(p);
	}

	if(participates_black)
	{
		player::ptr p(new player(player::BLACK));
		m_players.push_back(p);
	}

	BOOST_FOREACH(player::ptr &p, m_players.get_players())
		p->set_funds(m_initial_funds);

	m_players.set_first_player(0);

	//Do necessary steps for players which do not participate
	for(int x = 0; x < m_map->width(); x++)
	{
		for(int y = 0; y < m_map->height(); y++)
		{
			coord c(x, y);
			const unit::ptr &u = m_map->get_unit(c);
			terrain::ptr t = m_map->get_terrain(c);

			bool anyones_building = false;
			bool anyones_unit = false;

			BOOST_FOREACH(const player::ptr &p, m_players.get_players())
			{
				if(u && p->his_unit(u))
				{
					p->has_units(true);
					anyones_unit = true;
				}

				if(t && t->is_building() && p->his_building(t))
				{
					anyones_building = true;
				}
			}

			if(u && !u->is_dummy() && !anyones_unit) //There is a unit but the player of it doesn't participates
			{
				std::clog << "[game::start_game] Player of the unit at (" << x << "|" << y << ") doesn't participates the game. Deleting unit" << std::endl;
				m_map->delete_unit(c);
			}

			if(t && t->is_building() && t->extra() != terrain::WHITE && !anyones_building) //t is a building, isn't neutral and the player of it doesn't participates
			{
				std::clog << "[game::start_game] Player of the terrain at (" << x << "|" << y << ") doesn't participates the game. Changing color to white" << std::endl;
				t->extra(terrain::WHITE);
			}
		}
	}
}

void game::start_turn()
{
	for(int x = 0; x < 30; x++)
	{
		for(int y = 0; y < 20; y++)
		{
			coord c(x, y);
			unit::ptr u = m_map->get_unit(c);
			terrain::ptr t = m_map->get_terrain(c);

			if(u && u->color() == this->get_active_player()->get_unit_color())
			{
				u->begin_turn();

				if(u->remove())
					m_map->delete_unit(c);

				if(game_mechanics::can_supply(this->get_map(), c))
					this->supply_units(c);
			}

			if(t && t->is_building() && t->extra() == this->get_active_player()->get_building_color())
			{
				building::ptr b = boost::dynamic_pointer_cast<building>(t);
				assert(b != NULL);

				if(m_map->get_unit(c) && b->can_supply() && b->can_supply(u->get_environment()))
					this->supply_unit_from_building(c);
			}
		}
	}

	this->get_active_player()->add_funds(m_map->num_buildings(this->get_active_player()->get_color())*m_fpb);
	this->get_active_player()->start_turn();

	this->check_for_defeat();
}

void game::end_turn()
{
	if(this->move_active())
		this->cancel_unit_move();

	this->get_active_player()->end_turn();

	this->check_for_defeat();

	m_players.next();

	m_active_move.reset();
}


//display::scene::ptr game::get_prepared_scene()
//{
//	display::scene::ptr scene = m_map->scene();
//
////	if(m_active_move.active)
////	{
//////		scene->move_unit(m_active_move.from.x, m_active_move.from.y, m_active_move.to.x, m_active_move.to.y, true);
////		scene->get_unit(m_active_move.to.x, m_active_move.to.y) = scene->get_unit(m_active_move.from.x, m_active_move.from.y);
////	}
//
//	return scene;
//}

void game::check_for_defeat()
{
	BOOST_FOREACH(const player::ptr &p, m_players.get_players())
	{
		game_mechanics::defeat_type defeated = game_mechanics::beaten(this->get_map(), p);

		if(defeated != game_mechanics::NOT_BEATEN)
		{
			if(defeated == game_mechanics::HQ_CAPTURED)
			{
				for(int x = 0; x < m_map->width(); x++)
				{
					for(int y = 0; y < m_map->height(); y++)
					{
						coord c(x, y);
						const unit::ptr &u = this->get_map()->get_unit(c);
						const terrain::ptr &t = this->get_map()->get_terrain(c);

						if(u != NULL && p->his_unit(u))
							m_map->delete_unit(c);

						//We assume that the active player has defeated the player
						if(t != NULL && t->is_building() && p->his_building(t))
							t->extra(this->get_active_player()->get_building_color());
					}
				}

				m_signal_player_defeated(p, this->get_active_player(), game_mechanics::HQ_CAPTURED);
			}
			else if(defeated == game_mechanics::ALL_UNITS_BEATEN)
			{
				for(int x = 0; x < m_map->width(); x++)
				{
					for(int y = 0; y < m_map->height(); y++)
					{
						coord c(x, y);
						const terrain::ptr &t = this->get_map()->get_terrain(c);

						if(t != NULL && t->is_building() && p->his_building(t))
						{
							if(t->type() == terrain::HEADQUARTER && (t->extra() != terrain::WHITE && t->extra() != terrain::LAUNCHED)) // A white HQ is a silo
								t->type(terrain::CITY);

							t->extra(terrain::WHITE);
						}
					}
				}

				m_signal_player_defeated(p, this->get_active_player(), game_mechanics::ALL_UNITS_BEATEN);
			}

			m_players.erase(p);

			if(this->get_active_player() == p)
				this->end_turn();

			this->check_for_game_end();
		}
	}
}

void game::check_for_game_end()
{
	if(m_players.get_players().size() == 1)
		m_signal_game_finished(this->get_active_player());
}


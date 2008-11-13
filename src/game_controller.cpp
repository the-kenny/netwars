#include "game_controller.h"

#include "map_loader/map_loader.h"
#include "gui/buy_menu.h"
#include "gui/gui_fabric.h"
#include "game_mechanics/fire_range.h"
#include "game_mechanics/damage_calc.h"
#include "game_mechanics/attack_utilities.h"
#include "game_mechanics/prices.h"
#include "game_mechanics/explosion_damage.h"
#include "game_mechanics/player_utilities.h"
#include "game_mechanics/path.h"
#include "game_mechanics/unload_utilities.h"
#include "game_mechanics/supply_mechanics.h"
#include "coord.h"

#include <boost/bind.hpp>

//TODO(#4): Check ranges for all actions
using namespace aw;

game_controller::game_controller()
{
	m_gamestate = IDLE;

//	m_path = game_mechanics::path::ptr(new game_mechanics::path);
	m_traverse = game_mechanics::traverse::ptr(new game_mechanics::traverse);
}

void game_controller::start_game(const game::ptr &game)
{
	m_game = game;
	m_game->start_game();
	m_game->start_turn();

	this->update_display();
}

void game_controller::end_turn()
{
	m_game->end_turn();

	m_gamestate = IDLE;
	m_highlighted_area.clear();

	this->update_display();
}

void game_controller::start_turn()
{
	m_game->start_turn();
	this->update_display();
}

void game_controller::click(const coord &pos, int key)
{
	if(!m_game->get_unit(pos) && !terrain::is_building(m_game->get_terrain(pos)->type()))
		this->on_free_click(pos, key);
	else if(m_game->get_unit(pos))
		this->on_unit_click(pos, key);
	else if(terrain::is_building(m_game->get_terrain(pos)->type()))
		this->on_building_click(pos, key);

	this->update_display();
}

void game_controller::mouse_hover_changed(const coord &pos)
{
	if(m_gamestate != IDLE)
	{
		if(m_gamestate == MOVING)
		{
			if(m_selection == pos)
			{
				m_path->reset();
				this->update_display();
			}
			else if((pos.x != m_selection.x || pos.y != m_selection.y) && m_traverse->get_rest_mp(pos.x, pos.y) != -1)
			{
				m_path->calculate(m_game->get_map(), m_traverse, m_selection, pos);
				this->update_display();
			}
		}
		else if(m_gamestate == ATTACKING)
		{
			terrain::ptr t(m_game->get_terrain(m_selection));
			if(t->type() == terrain::HEADQUARTER && t->extra() == terrain::WHITE) //White HQ is a silo
			{
				area a(area::create(pos, boost::dynamic_pointer_cast<silo>(m_game->get_terrain(m_selection))->get_range()));
				m_highlighted_area.assign(a);

				this->update_display();
			}
		}
	}
}

void game_controller::on_unit_click(const coord &pos, int key)
{
	unit::ptr unit = m_game->get_unit(pos);
	if(key == 1) //Left mouse button
	{
		if(m_gamestate == IDLE)
		{
			if(m_game->get_active_player()->his_unit(unit) && !unit->moved()) //Not moved unit of the actual player was clicked
			{
				m_path = game_mechanics::path::ptr(new game_mechanics::path(pos));
				m_traverse = game_mechanics::traverse::ptr(new game_mechanics::traverse);

				m_traverse->reset();
				m_traverse->calculate(m_game->get_map(), pos);
				m_highlighted_area.assign(m_traverse->get_coordinates());

				m_gamestate = MOVING;
				m_selection = pos;
			}
		}
		else if(m_gamestate == ATTACKING)
		{
			if(boost::dynamic_pointer_cast<silo>(m_game->get_terrain(m_selection)) != NULL) //Target for a silo was selected
			{
				m_game->launch_silo(m_selection, pos);

				if(m_game->move_active())
					m_game->complete_unit_move(*m_path);
				else
					m_game->get_unit(m_selection)->set_moved();

				m_gamestate = IDLE;
				m_selection.reset();
			}
			else if(!m_game->get_active_player()->his_unit(unit) && game_mechanics::attack_utilities::can_attack(m_game->get_unit(m_selection), unit) && m_highlighted_area.contains(pos)) //An opposite unit is selected to attack
			{
				m_game->attack_unit(m_selection, pos);

				if(m_game->move_active())
					m_game->complete_unit_move(*m_path);
				else
					m_game->get_unit(m_selection)->set_moved();

				m_gamestate = IDLE;
			}
			else if(m_selection == pos && m_game->get_active_player()->his_unit(unit) && unit->can_explode()) //Menu for explosion will be displayed
			{
				if(m_game->move_active())
					m_game->complete_unit_move(*m_path);

				m_game->explode(m_selection);

				m_gamestate = IDLE;
				m_selection.reset();
			}
		}
		else if(m_gamestate == MOVING)
		{
			if(pos == m_selection) //Selected unit was clicked again
			{
				this->process_action(this->show_actions(pos), pos);
			}
			else if(m_highlighted_area.contains(pos) && m_game->get_active_player()->his_unit(unit))
			{
				if(m_game->get_active_player()->his_unit(unit) && unit->is_transporter()) //Transporter is the target
				{
					transporter::ptr t = boost::dynamic_pointer_cast<transporter>(unit);

					if(t->can_load(m_game->get_unit(m_selection)->type()))
					{
						gui::unit_action_menu *menu = gui::create_unit_action_menu();

						menu->add_action(gui::unit_action_menu::LOAD);

						int ret = menu->run();

						if(ret == gui::unit_action_menu::LOAD)
						{
							m_game->load_unit(pos, m_selection, *m_path);

							m_gamestate = IDLE;
							m_selection.reset();
						}
						else
						{
							if(m_game->move_active())
								m_game->cancel_unit_move();

							m_gamestate = IDLE;
							m_selection.reset();
						}
					}
				}
				else if(m_game->get_unit(m_selection)->type() == unit->type()) //Combine units
				{
					//TODO: Überprüfen, wann sich Einheiten zusammenfügen können
					//if(m_game->get_unit(pos)->life() < m_game->get_unit(pos)->max_life())
					if(m_game->get_unit(m_selection)->get_hp() < m_game->get_unit(m_selection)->max_hp() || unit->get_hp() < unit->max_hp())
					{
						gui::unit_action_menu *m = gui::create_unit_action_menu();
						m->add_action(gui::unit_action_menu::JOIN);

						gui::unit_action_menu::actions ret = m->run();

						if(ret == gui::unit_action_menu::JOIN)
						{
							m_game->join_units(m_selection, pos, *m_path);

							m_gamestate = IDLE;
							m_selection.reset();
						}
					}
				}
			}
			else if(m_highlighted_area.contains(pos) && !m_game->get_active_player()->his_unit(unit))
			{
				if(m_game->get_unit(m_selection)->can_run_over()) //Oozium can overrun oppotiste units
				{
					gui::unit_action_menu *m = gui::create_unit_action_menu();
					m->add_action(gui::unit_action_menu::ATTACK);

					gui::unit_action_menu::actions ret = m->run();

					if(ret == gui::unit_action_menu::ATTACK)
					{
						m_game->knock_down(m_selection, pos);

						m_game->begin_unit_move(m_selection, pos); //Ugly

						if(m_game->move_active())
							m_game->complete_unit_move(*m_path);
						else
							m_game->get_unit(m_selection)->set_moved();

						m_gamestate = IDLE;
						m_selection.reset();
					}
				}
				else if(unit->is_dummy()) //Dummy-Units will be moved out of way as long as a unit is on their field
				{
					m_game->begin_unit_move(m_selection, pos);
					this->process_action(this->show_actions(pos), pos);
				}
			}
		}
		else if(m_gamestate == REPAIRING)
		{
			if(m_game->get_active_player()->his_unit(unit) && m_game->get_unit(m_selection)->can_supply(unit->get_environment())) //Unit will be repaired
			{
				m_game->repair_unit(m_selection, pos);

				if(m_game->move_active())
					m_game->complete_unit_move(*m_path);
				else
					m_game->get_unit(m_selection)->set_moved();

				m_gamestate = IDLE;
				m_selection.reset();
			}
		}
	}
	else if(key == 3) //Right click
	{
		if(m_gamestate == IDLE)
		{
			if(unit->can_fire())
			{
				game_mechanics::fire_range_preview p;
				p.calculate(m_game->get_map(), pos.x, pos.y);
				m_highlighted_area.assign(p.get_coordinates());

				m_gamestate = DISPLAYING;
				m_selection = pos;
			}
		}
		else
		{
			if(m_game->move_active())
				m_game->cancel_unit_move();

			m_gamestate = IDLE;
			m_selection.reset();
		}
	}
}

void game_controller::on_building_click(const coord &pos, int key)
{
	terrain::ptr terrain(m_game->get_terrain(pos));

	if(key == 1)
	{
		if(m_gamestate == IDLE)
		{
			//Wenn dort kein Unit steht und es sein Gebäude ist
			if(!m_game->get_unit(pos) && m_game->get_terrain(pos)->is_building() && m_game->get_active_player()->his_building(m_game->get_terrain(pos)))
			{
				unit::workshops shop;
				switch(m_game->get_terrain(pos)->type())
				{
					case terrain::BASE:
						shop = unit::BASE;
						break;
					case terrain::AIRPORT:
						shop = unit::AIRPORT;
						break;
					case terrain::PORT:
						shop = unit::PORT;
						break;

					//Kann keine Einheiten produzieren
					default:
						return;
				}

				gui::buy_menu *d = gui::create_buy_menu();
				d->set_player(m_game->get_active_player());
				d->set_workshop(shop);

				d->on_buy_signal().connect(boost::bind(&game::buy_unit, m_game, pos, _1));
				d->run();
			}
		}
		else if(m_gamestate == ATTACKING)
		{
			if(boost::dynamic_pointer_cast<silo>(m_game->get_terrain(m_selection)) != NULL)
			{
				if(m_game->move_active())
					m_game->complete_unit_move(*m_path);

				m_game->launch_silo(m_selection, pos);

				if(m_game->move_active())
					m_game->complete_unit_move(*m_path);
				else
					m_game->get_unit(m_selection)->set_moved();

				m_gamestate = IDLE;
				m_selection.reset();
			}
		}
		else if(m_gamestate == MOVING)
		{
			if(m_traverse->in_range(pos))
			{
				m_game->begin_unit_move(m_selection, pos);
				this->process_action(this->show_actions(pos), pos);
			}
		}
		else if(m_gamestate == UNLOADING)
		{
			transporter::ptr t = boost::dynamic_pointer_cast<transporter>(m_game->get_unit(m_selection));
			assert(t != NULL);

			if(m_highlighted_area.contains(pos))
			{
				assert(game_mechanics::can_unload(m_game->get_map(), m_selection, m_unload_index));

				m_game->unload_unit(m_selection, *m_path, pos, m_unload_index);

				if(t->can_unload())
				{
					this->process_action(gui::unit_action_menu::UNLOAD, m_selection);
				}
				else
				{
					m_gamestate = IDLE;
					m_selection.reset();
				}
			}
		}
	}
	else if(key == 3)
	{
		if(m_game->move_active())
			m_game->cancel_unit_move();

		m_gamestate = IDLE;
		m_selection.reset();
	}
}

void game_controller::on_free_click(const coord &pos, int key)
{
	if(key == 1)
	{
		if(m_gamestate == ATTACKING)
		{
			if(boost::dynamic_pointer_cast<silo>(m_game->get_terrain(m_selection)) != NULL)
			{
				m_game->launch_silo(m_selection, pos);

				if(m_game->move_active())
					m_game->complete_unit_move(*m_path);
				else
					m_game->get_unit(m_selection)->set_moved();

				m_gamestate = IDLE;
				m_selection.reset();
			}
		}
		else if(m_gamestate == MOVING)
		{
			if(m_traverse->in_range(pos))
			{
				m_game->begin_unit_move(m_selection, pos);
				this->process_action(this->show_actions(pos), pos);
			}
		}
		else if(m_gamestate == UNLOADING)
		{
			transporter::ptr t = boost::dynamic_pointer_cast<transporter>(m_game->get_unit(m_selection));
			assert(t != NULL);

			if(m_highlighted_area.contains(pos))
			{
				assert(game_mechanics::can_unload(m_game->get_map(), m_selection, m_unload_index));

				m_game->unload_unit(m_selection, *m_path, pos, m_unload_index);

				if(t->can_unload())
				{
					this->process_action(gui::unit_action_menu::UNLOAD, m_selection);
				}
				else
				{
					m_gamestate = IDLE;
					m_selection.reset();
				}
			}
		}
	} // key == 1
	else if(key == 3)
	{
		if(m_game->move_active())
			m_game->cancel_unit_move();

		m_gamestate = IDLE;
		m_selection.reset();
	}
}

gui::unit_action_menu::actions game_controller::show_actions(const coord &pos)
{
	unit::ptr unit = m_game->get_unit(pos);

	if(!unit)
		throw std::runtime_error("No unit at give coordinates");

	bool can_attack = false;

//	if(unit->get_attack_distance() > 0 && unit->moved())
	if(unit->get_attack_distance() > 0 && m_game->move_active())
	{
		can_attack = false;
	}
	else
	{
		game_mechanics::fire_range f;
		f.calculate(m_game->get_map(), pos.x, pos.y);

		BOOST_FOREACH(const coord &c, f.get_victims(m_game->get_map(), m_game->get_active_player()))
		{
			if(game_mechanics::attack_utilities::can_attack(unit, m_game->get_unit(c)))
			{
				can_attack = true;
				break;
			}
		}
	}

	bool can_capture = false;
	bool can_launch = false;

	if(unit->can_capture())
	{
		terrain::ptr terr = m_game->get_terrain(pos);
		silo::ptr silo(boost::dynamic_pointer_cast<silo>(terr));

		if(terr->type() == terrain::HEADQUARTER && silo)
		{
			if(!silo->has_fired())
				can_launch = true;
		}
		else if(terr->is_building() && !m_game->get_active_player()->his_building(terr))
		{
			can_capture = true;
		}
	}

	bool can_unload = false;
	if(unit->is_transporter())
	{
		transporter::ptr t = boost::dynamic_pointer_cast<transporter>(unit);

		if(t->can_unload())
		{
			BOOST_FOREACH(const unit::ptr &u, t->get_unit_list())
			{
				if(m_game->on_map(pos) && m_game->get_terrain(pos)->can_pass(u->move_type()))
				{
					if(m_game->on_map(coord(pos.x+1, pos.y)) && m_game->get_terrain(coord(pos.x+1, pos.y))->can_pass(u->move_type()))
						can_unload = true;
					if(m_game->on_map(coord(pos.x-1, pos.y)) && m_game->get_terrain(coord(pos.x-1, pos.y))->can_pass(u->move_type()))
						can_unload = true;
					if(m_game->on_map(coord(pos.x, pos.y-1)) && m_game->get_terrain(coord(pos.x, pos.y-1))->can_pass(u->move_type()))
						can_unload = true;
					if(m_game->on_map(coord(pos.x, pos.y+1)) && m_game->get_terrain(coord(pos.x, pos.y+1))->can_pass(u->move_type()))
						can_unload = true;
				}
			}
		}
	}

	bool can_explode = unit->can_explode();
	bool can_supply = game_mechanics::can_supply(m_game->get_map(), pos);
	bool can_repair = game_mechanics::can_repair(m_game->get_map(), pos, m_game->get_active_player()->get_funds());

	gui::unit_action_menu *menu = gui::create_unit_action_menu();

	if(can_attack)
		menu->add_action(gui::unit_action_menu::ATTACK);
	if(can_launch)
		menu->add_action(gui::unit_action_menu::LAUNCH);
	if(can_capture)
		menu->add_action(gui::unit_action_menu::CAPTURE);
	if(can_unload)
		menu->add_action(gui::unit_action_menu::UNLOAD);
	if(can_explode)
		menu->add_action(gui::unit_action_menu::EXPLODE);

	if(can_repair)
		menu->add_action(gui::unit_action_menu::REPAIR);
	else if(can_supply)
		menu->add_action(gui::unit_action_menu::SUPPLY);


	if(unit->can_hide())
	{
		if(unit->is_hidden())
			menu->add_action(gui::unit_action_menu::APPEAR);
		else
			menu->add_action(gui::unit_action_menu::HIDE);
	}

	menu->add_action(gui::unit_action_menu::WAIT);

	return menu->run();
}

void game_controller::process_action(gui::unit_action_menu::actions action, const coord &pos)
{
	unit::ptr unit = m_game->get_unit(pos);

	if(action == gui::unit_action_menu::WAIT)
	{
		m_gamestate = IDLE;

		if(m_game->move_active())
			m_game->complete_unit_move(*m_path);
		else
			unit->set_moved();

		m_selection.reset();
	}
	else if(action == gui::unit_action_menu::ATTACK)
	{
		m_gamestate = ATTACKING;
		m_selection = pos;

		game_mechanics::fire_range f;
		f.calculate(m_game->get_map(), pos.x, pos.y);
		m_highlighted_area.assign(f.get_victims(m_game->get_map(), m_game->get_active_player()));
	}
	else if(action == gui::unit_action_menu::CAPTURE)
	{
		m_game->capture_building(pos);

		m_gamestate = IDLE;

		if(m_game->move_active())
			m_game->complete_unit_move(*m_path);
		else
			unit->set_moved();

		m_selection.reset();

		this->update_display();
	}
	else if(action == gui::unit_action_menu::UNLOAD) //Big and ugly
	{
		transporter::ptr transport_unit = boost::dynamic_pointer_cast<transporter>(m_game->get_unit(pos));

		std::vector<unit::ptr> unloadable_units;
		std::vector<std::size_t> unloadable_unit_indices;

		typedef std::pair<std::size_t, unit::ptr> pair_t;
		BOOST_FOREACH(pair_t p, game_mechanics::get_unloadable_units(m_game->get_map(), pos))
		{
			unloadable_units.push_back(p.second);
			unloadable_unit_indices.push_back(p.first);
		}

		gui::unit_unload_menu *um = gui::create_unload_menu();
		um->set_units(unloadable_units);
		int ret = um->run();

		m_highlighted_area.clear();

		if(ret != -1)
		{
			m_unload_index = unloadable_unit_indices.at(ret);
			m_gamestate = UNLOADING;

			m_highlighted_area.assign(game_mechanics::get_unload_coordinates(m_game->get_map(), pos, m_unload_index));

			m_selection = pos;
		}
		else
		{
			if(m_game->move_active())
				m_game->cancel_unit_move();

			m_gamestate = IDLE;
			m_selection.reset();
		}
	}
	else if(action == gui::unit_action_menu::HIDE)
	{
		unit->hide();

		m_gamestate = IDLE;

		if(m_game->move_active())
			m_game->complete_unit_move(*m_path);
		else
			unit->set_moved();

		m_selection.reset();

		this->update_display();
	}
	else if(action == gui::unit_action_menu::APPEAR)
	{
		unit->appear();

		m_gamestate = IDLE;

		if(m_game->move_active())
			m_game->complete_unit_move(*m_path);
		else
			unit->set_moved();

		m_selection.reset();

		this->update_display();
	}
	else if(action == gui::unit_action_menu::EXPLODE)
	{
		m_gamestate = ATTACKING;
		m_selection = pos;

		game_mechanics::explosion_damage d;
		d.calculate(m_game->get_map(), pos, unit->get_explosion_range(), unit->get_explosion_damage());

		m_highlighted_area.assign(d.get_area());
	}
	else if(action == gui::unit_action_menu::LAUNCH)
	{
		m_gamestate = ATTACKING;
		m_selection = pos;

		m_highlighted_area.clear();
	}
	else if(action == gui::unit_action_menu::SUPPLY)
	{
		m_game->supply_units(pos);

		m_gamestate = IDLE;

		if(m_game->move_active())
			m_game->complete_unit_move(*m_path);
		else
			unit->set_moved();

		this->update_display();
	}
	else if(action == gui::unit_action_menu::REPAIR)
	{
		m_gamestate = REPAIRING;
		m_selection = pos;

//		area a(area::create(pos, 1));
//		a.erase(pos);

//		BOOST_FOREACH(const coord &c, a)
//		{
//			if(m_game->on_map(c))
//			{
//				const unit::ptr u = m_game->get_unit(c);
//				//if(!u || !m_game->get_active_player()->his_unit(u) || !unit->can_supply(u->get_environment()) || u->life() >= u->max_life())
//				if(!u || !m_game->get_active_player()->his_unit(u) || !unit->can_repair(u->get_environment()) || u->life() >= u->max_life())
//					a.erase(c);
//			}
//		}

		area a = game_mechanics::get_repair_coordinates(m_game->get_map(), pos, m_game->get_active_player()->get_funds());

		m_highlighted_area.assign(a);
	}
	else if(action == gui::unit_action_menu::CANCEL)
	{
//		m_gamestate = IDLE;

		if(m_game->move_active())
			m_game->cancel_unit_move();
//		m_selection.reset();

		this->update_display();
	}
}

void game_controller::update_display()
{
	display::scene::ptr scene = m_game->get_prepared_scene();

	if(m_gamestate != IDLE)
	{
		if(!m_highlighted_area.empty())
			scene->highlighted(m_highlighted_area);

		if(m_selection)
			scene->highlighted_unit(m_selection);

		if(m_gamestate == MOVING && m_path && !m_path->get_path().empty())
			scene->path(m_path->get_path());
	}

	m_signal_scene_changed(scene);
}

#ifndef AW_UNIT_H
	#define AW_UNIT_H

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <list>

namespace aw
{
	class unit_loader;

	//TODO: Eine Art Attributsystem als Ersatz der ganzen Variablen entwickeln
	class unit
	{
		public:
			typedef boost::shared_ptr<unit> ptr;

			friend class aw::unit_loader; //ugly

			typedef std::string types;
			const static unit::types TYPE_NONE;
			
			enum colors { UNDEF, RED, BLUE, GREEN, YELLOW, BLACK };
			enum extras { LIVE, LOW_AMMO, LOW_FUEL, HIDDEN, LOADED, CAPTURE, MINUS };
			enum environments { GROUND, AIR, WATER };
			enum move_types { FOOT, MECHANICAL, TREAD, TIRES, FLY, SWIM, TRANSPORT, OOZ, PIPE };
			enum victim_t { VIC_INFANTRY, VIC_VEHICLE, VIC_COPTER, VIC_PLANE, VIC_SHIP, VIC_SUBMARINE, VIC_OOZ };
			enum workshops { NOWHERE, BASE, AIRPORT, PORT };

			class weapon
			{
				public:
					bool active;

					int ammo;
					int max_ammo;

					int range;
					int distance;

//					std::vector<std::string> m_victims;

					weapon()
					{
						active = false;
						ammo = 0;
						max_ammo = 0;
						range = -1;
						distance = -1;
					}

					void join(const weapon &o)
					{
						if(ammo + o.ammo <= max_ammo)
							ammo += o.ammo;
						else
							ammo = max_ammo;
					}

					void reset()
					{
						ammo = max_ammo;
					}
			};

			unit(types type, colors color, move_types mtype = TIRES)
			: m_type(type), m_color(color), m_move_type(mtype)
			{
				m_max_life = -1;
				m_life = -1;

				m_has_shot = false;

				m_max_fuel = -1;
				m_fuel = -1;
				m_fuel_per_day = -1; //Daily fuel usage
				m_remove_without_fuel = false;

				m_movement_range = -1;
				m_moved = false;

				m_can_capture = false;

				m_can_hide = false;
				m_is_hidden = false;
				m_fuel_per_day_hidden = -1;

				m_can_load = false;

				m_can_supply = false;
				m_can_repair = false;
				m_repair_points = -1;

				m_can_counterattack = false;
				m_run_over = false;

				m_can_explode = false;
				m_explosion_range = -1;
				m_explosion_damage = -1;

				this->reset();
			}

			virtual ~unit()
			{
			}

			virtual bool is_dummy() const { return false; }

			void set_hp(int hp) { m_life = hp; }
			int get_hp() const { return m_life; }
			int max_hp() const { return m_max_life; }

			float get_hp_as_float() const { return m_life; }
			void set_hp_as_float(float hp) { m_life = hp; }

			void apply_damage(float damage)
			{
				m_life -= damage;
				if(m_life <= 0.0)
					m_life = 0.0;
			}

			int ammo() const
			{
				if(main_weapon_useable())
					return main_weapon.ammo;
				else if(alt_weapon_useable())
					return alt_weapon.ammo;
				else
					return 0;
			}

			int max_main_ammo() const
			{
				return main_weapon.max_ammo;
			}

			int max_alt_ammo() const
			{
				return alt_weapon.max_ammo;
			}

			int movement_range() const { return m_movement_range;}

			bool moved() const { return m_moved; }
			void set_moved(bool moved = true) { m_moved = moved; }
			virtual void reset_move()
			{
				m_moved = false;
				m_has_shot = false;
			}

			int fuel() const { return m_fuel; }
			int max_fuel() const { return m_max_fuel; }
			bool low_fuel() { return (static_cast<double>(m_fuel)/static_cast<double>(m_max_fuel) < 0.5); /*Weniger als die Hälfte der Maximalfüllung*/ }

			bool remove()
			{
				if(m_remove_without_fuel && m_fuel <= 0)
					return true;
				else
					return false;
			}

			bool has_shot() { return m_has_shot; }
			void reset_shot() { m_has_shot = false; }

			bool low_ammo()
			{
				if(main_weapon_useable())
				{
					if(main_weapon.max_ammo <= (main_weapon.ammo/2) && main_weapon.ammo > 0)
						return true;
				}
				else if(alt_weapon_useable())
				{
					if(alt_weapon.max_ammo <= (alt_weapon.ammo/2) && alt_weapon.ammo > 0)
						return true;
				}

				return false;
			}

			int get_attack_distance() const
			{
				if(main_weapon_useable())
					return main_weapon.distance;
				else if(alt_weapon_useable())
					return alt_weapon.distance;
				else
					return -1;
			}

			int get_attack_range() const
			{
				if(main_weapon_useable())
					return main_weapon.range;
				else if(alt_weapon_useable())
					return alt_weapon.range;
				else
					return -1;
			}

			bool main_weapon_useable() const
			{
				if(main_weapon.active && (main_weapon.ammo > 0 || main_weapon.ammo == -1))
					return true;
				else
					return false;
			}

			bool alt_weapon_useable() const
			{
				if(alt_weapon.active && (alt_weapon.ammo > 0 || alt_weapon.ammo == -1))
					return true;
				else
					return false;
			}

			move_types get_move_type() const { return m_move_type; }
			types type() const { return m_type; }
			colors color() const { return m_color; }
			move_types move_type() const { return m_move_type; }
			bool can_capture() const { return m_can_capture;}
//			victim_t victim_type() const { return m_victim_type; }
			bool can_counterattack() const { return m_can_counterattack; }

			bool can_hide() const { return m_can_hide; }
			bool is_hidden() const { return m_is_hidden; }
			bool can_run_over() const { return m_run_over; }

			bool can_supply() const { return m_can_supply; }
			bool can_repair() const { return m_can_repair; }
			int get_repair_points() const { return m_repair_points; }

			bool can_supply(environments e) const
			{
				return (m_can_supply && std::find(m_supply_types.begin(), m_supply_types.end(), e) != m_supply_types.end());
			}

			bool can_repair(environments e) const
			{
				return (m_can_repair && std::find(m_supply_types.begin(), m_supply_types.end(), e) != m_supply_types.end());
			}

			bool can_explode() const { return m_can_explode; }
			int get_explosion_range() const { return m_explosion_range; }
			int get_explosion_damage() const { return m_explosion_damage; }

			environments get_environment() const
			{
				switch(this->get_move_type())
				{
					case FOOT:
					case MECHANICAL:
					case TREAD:
					case TIRES:
					case OOZ:
					case PIPE:
						return GROUND;
						break;

					case FLY:
						return AIR;
						break;

					case SWIM:
					case TRANSPORT:
						return WATER;
						break;

					default:
						throw std::runtime_error(": Bad move_type");
				}
			}

			void hide()
			{
				if(!m_can_hide)
					throw std::runtime_error("Unit can't hide");

				std::swap(m_fuel_per_day, m_fuel_per_day_hidden);
				m_is_hidden = true;

//				std::clog << "Unit is now hidden.\n"
//							<< "Fuel-Usage: " << m_fuel_per_day << std::endl;
			}

			void appear()
			{
				if(!m_can_hide)
					throw std::runtime_error("Unit can't appear");

				std::swap(m_fuel_per_day, m_fuel_per_day_hidden);
				m_is_hidden = false;

//				std::clog << "Unit is now no more hidden.\n"
//							<< "Fuel-Usage: " << m_fuel_per_day << std::endl;
			}

			void move(int costs)
			{
				std::clog << "[unit::move] Benzin wird verbraucht: " << costs << std::endl;

				if(m_moved)
					throw std::runtime_error("[unit::move] Unit wurde bereits bewegt");

				if(m_movement_range - costs < 0 || m_fuel - costs < 0)
					throw std::runtime_error("[unit::move] Weg ist zu weit");

				m_fuel -= costs;

				m_moved = true;
			}

			bool can_fire()
			{
				if(main_weapon_useable() || alt_weapon_useable())
					return true;
				else
					return false;
			}

			void fire()
			{
				if(main_weapon_useable())
				{
					if(main_weapon.ammo > 0)
						main_weapon.ammo--;
				}
				else if(alt_weapon_useable())
				{
					if(alt_weapon.ammo > 0)
						alt_weapon.ammo--;
				}
				else
				{
					throw;
				}

				m_has_shot = true;
			}

			void begin_turn()
			{
				m_fuel -= m_fuel_per_day;
				this->reset_move();
			}

			void join(const unit &o)
			{
				if(m_life + o.m_life <= m_max_life)
					m_life += o.m_life;
				else
					m_life = m_max_life;

				if(m_fuel + o.m_fuel <= m_max_fuel)
					m_fuel += o.m_fuel;
				else
					m_fuel = m_max_fuel;

					main_weapon.join(o.main_weapon);
					alt_weapon.join(o.alt_weapon);
			}

			void reset()
			{
				m_life = m_max_life;
//				m_ammo = m_max_ammo;
				m_fuel = m_max_fuel;

				m_has_shot = false;
				m_moved = false;

				if(m_is_hidden)
					this->appear();

				main_weapon.reset();
				alt_weapon.reset();
			}

			void supply()
			{
				main_weapon.reset();
				alt_weapon.reset();

				m_fuel = m_max_fuel;
			}

			void repair(int i)
			{
				if(m_life + i <= m_max_life)
					m_life += i;
				else
					m_life = m_max_life;
			}

			std::string get_name() const
			{
				return unit::get_name(m_type);
			}

			static std::string get_name(const unit::types t)
			{
				return t;
			}

			bool is_transporter()
			{
				return m_can_load;
			}
//
//			bool operator==(const std::string &o)
//			{
//				return this->type() == o;
//			}
//
//			bool operator!=(const std::string &o)
//			{
//				return !(*this == o);
//			}


		protected:
			int m_max_life;
			float m_life;

			bool m_has_shot;

			int m_max_fuel;
			int m_fuel;
			int m_fuel_per_day; //Daily fuel usage
			bool m_remove_without_fuel;

			int m_movement_range;
			bool m_moved;

			bool m_can_capture;
			bool m_can_hide;

			bool m_can_supply;
			bool m_can_repair;
			int m_repair_points;
			std::list<unit::environments> m_supply_types;

			bool m_is_hidden;
			int m_fuel_per_day_hidden;

			victim_t m_victim_type;

			bool m_can_load;

			bool m_can_counterattack;
			bool m_run_over;

			bool m_can_explode;
			int m_explosion_range;
			int m_explosion_damage;


			weapon main_weapon;
			weapon alt_weapon;


//			std::vector<weapon> m_weapons;

//			std::map<victim_t, int> m_first_weapon_efficiency;
//			std::map<victim_t, int> m_second_weapon_efficiency;

        //NOTE: Very very bad!
		public:
			types m_type;					//Should not be changed
			colors m_color;					//Should not be changed
			move_types m_move_type;	//Should not be changed
	};

	class dummy_unit: public unit
	{
		public:
			dummy_unit(types type, move_types mtype = SWIM)
			: unit(type, UNDEF, mtype)
			{
			}

			virtual bool is_dummy() const { return true; }
	};
}

#endif

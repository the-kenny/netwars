#include "player.h"

#include <boost/lexical_cast.hpp>

namespace aw
{
	player::player(colors c)
	: m_funds(0), m_color(c), m_has_units(false)
	{

	}

//	void player::capture_building(int fpr)
//	{
//		if(m_funds_per_turn + fpr < 0)
//			m_funds_per_turn = 0;
//		else
//			m_funds_per_turn += fpr;
//	}

	unit::colors player::get_unit_color() const
	{
		unit::colors color = unit::RED;
		switch(m_color)
		{
			case RED:
				color = unit::RED;
				break;

			case BLUE:
				color = unit::BLUE;
				break;

			case GREEN:
				color = unit::GREEN;
				break;

			case YELLOW:
				color = unit::YELLOW;
				break;

			case BLACK:
				color = unit::BLACK;
				break;
		}

		return color;
	}

	terrain::extras player::get_building_color() const
	{
		terrain::extras color = terrain::NONE;

		switch(m_color)
		{
			case RED:
				color = terrain::RED;
				break;
			case BLUE:
				color = terrain::BLUE;
				break;
			case GREEN:
				color = terrain::GREEN;
				break;
			case YELLOW:
				color = terrain::YELLOW;
				break;
			case BLACK:
				color = terrain::BLACK;
				break;
		}

		return color;
	}


	void player::start_turn()
	{
//		m_funds += m_funds_per_turn;
	}

	void player::end_turn()
	{

	}

	std::string player::get_color_string() const
	{
		std::string out;

		switch(this->get_color())
		{
			case player::RED:
				out = "Red";
				break;
			case player::BLUE:
				out = "Blue";
				break;
			case player::GREEN:
				out = "Green";
				break;
			case player::YELLOW:
				out = "Yellow";
				break;
			case player::BLACK:
				out = "Black";
				break;
		}

		return out;
	}

	void player::print(std::ostream &o) const
	{
		std::string out;
		out += "Farbe: " + this->get_color_string() + '\n';

		out += "Funds: " + boost::lexical_cast<std::string>(m_funds) + '\n';

		o << out;
	}

	std::ostream &operator<<(std::ostream &o, const player &p)
	{
		p.print(o);

		return o;
	}
}

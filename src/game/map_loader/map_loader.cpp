#include "map_loader.h"

#include <fstream>
#include <stdexcept>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "game/units/unit_loader.h"

namespace aw
{
	map_loader::map_loader(const std::string &filename)
	: m_filename(filename), m_size_x(-1), m_size_y(-1)
	{
	}

	//TODO(#2): Rewrite the map_loader
	 map_loader::loaded_map::ptr map_loader::load(const std::string &filename)
	{
		if(!m_map && filename != m_filename)
		{
			if(filename == "" && m_filename == "")
			{
				throw std::runtime_error("Kein Dateiname angegeben, Abbruch");
			}
			else if(filename != "")
			{
				m_filename = filename;
			}

			std::ifstream file(m_filename.c_str(), std::ios::in | std::ios::binary);

			if(file)
			{
				file.seekg(0, std::ios::end);
				std::size_t filesize = file.tellg();
				m_data.resize(filesize);

				file.seekg(0, std::ios::beg);
				file.read(&m_data.at(0), m_data.size());

				file.close();

				m_map =  loaded_map::ptr(new loaded_map);


				std::size_t position = this->read_header();

				switch(m_map_type)
				{
					case AWS:
						position = load_aws(position);
						break;
					case AWD:
						position = load_awd(position);
						break;
					case AWM:
					case AW2:
						position = load_awm(position);
						break;
				}

				this->read_metadata(position);
			}
			else
			{
				throw std::runtime_error("\"" + m_filename + "\" konnte nicht geöffnet werden.");
			}
		}

		return m_map;
	}

	std::size_t map_loader::read_header()
	{
		char header[10];
		std::copy(m_data.begin(), m_data.begin()+10, header);

		m_header = header;

		std::string spec = m_header.substr(0, 3);
		if(spec == "AWS")
			m_map_type = AWS;
		else if(spec == "AWD")
			m_map_type = AWD;
		else if(spec == "AWM")
			m_map_type = AWM;
		 else if(spec == "AW2")
			m_map_type = AW2;
		else
			throw std::runtime_error("Undefinierter Header, Abbruch");

		return 10;
	}

	std::size_t map_loader::load_aws(std::size_t position)
	{
		m_size_x = *reinterpret_cast<uint8_t*>(&m_data.at(position));
		position += sizeof(uint8_t);
		m_size_y = *reinterpret_cast<uint8_t*>(&m_data.at(position));
		position += sizeof(uint8_t);

		if(m_size_x != 30 || m_size_y != 20)
			throw std::runtime_error("Größe der geladenen Map ist nicht unterstützt.");

		m_tileset =  *reinterpret_cast<uint8_t*>(&m_data.at(position));
		position += sizeof(uint8_t);

		uint16_t data = 0;
		for(int i = 0; i < 600; i++)
		{
			data =  *reinterpret_cast<uint16_t*>(&m_data.at(position));
			position += sizeof(uint16_t);

			this->process_data(data, i/20, i%20);
		}

		for(int i = 0; i < 600; i++)
		{
			data =  *reinterpret_cast<uint16_t*>(&m_data.at(position));
			position += sizeof(uint16_t);

			this->process_data(data, i/20, i%20);
		}

		return position;
	}

	std::size_t map_loader::load_awd(std::size_t position)
	{
		m_size_x = 30;
		m_size_y = 20;

		m_tileset =  *reinterpret_cast<uint8_t*>(&m_data.at(position));
		position += sizeof(uint8_t);

		uint16_t data = 0;
		for(int i = 0; i < 600; i++)
		{
			data =  *reinterpret_cast<uint16_t*>(&m_data.at(position));
			position += sizeof(uint16_t);

			this->process_data(data, i/20, i%20);
		}

		for(int i = 0; i < 600; i++)
		{
			data =  *reinterpret_cast<uint16_t*>(&m_data.at(position));
			position += sizeof(uint16_t);

			this->process_data(data, i/20, i%20);
		}

		return position;
	}

	std::size_t map_loader::load_awm(std::size_t position)
	{
		m_size_x = 30;
		m_size_y = 20;

		uint16_t data = 0;
		for(int i = 0; i < 600; i++)
		{
			data =  *reinterpret_cast<uint16_t*>(&m_data.at(position));
			position += sizeof(uint16_t);

			this->process_data(data, i/20, i%20);
		}

		for(int i = 0; i < 600; i++)
		{
			data =  *reinterpret_cast<uint16_t*>(&m_data.at(position));
			position += sizeof(uint16_t);

			this->process_data(data, i/20, i%20);
		}

		return position;
	}

	void map_loader::read_metadata(std::size_t position)
	{
		uint32_t name_length = *reinterpret_cast<uint32_t*>(&m_data.at(position));
		position += sizeof(uint32_t);
		if(name_length > 0)
		{
			m_name.assign(&m_data.at(position), name_length);
			position += name_length;
		}

		uint32_t author_length = *reinterpret_cast<uint32_t*>(&m_data.at(position));
		position += sizeof(uint32_t);
		if(author_length > 0)
		{
			m_author.assign(&m_data.at(position), author_length);
			position += author_length;
		}

		uint32_t desc_length = *reinterpret_cast<uint32_t*>(&m_data.at(position));
		position += sizeof(uint32_t);
		if(desc_length > 0)
			m_description.assign(&m_data.at(position), desc_length);
	}

	void map_loader::process_data(uint16_t data, unsigned int x, unsigned int y)
	{
		if(data <= 499) //Terrains and Bases
		{
			const terrain::types terrain_t = convert_to_terrain_type(data);

			m_map->m_terrain[x][y]->type(terrain_t);
			m_map->m_terrain[x][y]->extra(convert_to_terrain_extra(data));

			if(terrain_t == terrain::SEGMENT_PIPE)
			{
				m_map->m_unit[x][y] = unit::ptr(new dummy_unit("segment_pipe"));

				return;
			}
		}
		else if(data >= 500 && data <= 899)
		{
			m_map->m_unit[x][y] = unit::ptr(new unit(convert_to_unit_type(data), convert_to_unit_color(data)));
		}
		else if(data >= 900 && data <= 1299)
		{
			throw std::runtime_error("Spezielle Gebäude (Kanonen etc.) sind (noch) nicht implentiert");
		}
	}

	terrain::types map_loader::convert_to_terrain_type(uint16_t value)
	{
		switch(value)
		{
			case 0:
				return terrain::PLAIN;
				break;
			case 60:
				return terrain::WATER;
				break;
			case 3:
				return terrain::RIVER;
				break;
			case 1:
				return terrain::STREET;
				break;
			case 32:
			case 2:
				return terrain::BRIDGE;
				break;
			case 16:
				return terrain::PIPE;
				break;
			case 226:
				return terrain::SEGMENT_PIPE;
				break;
			case 167:
				return terrain::WRECKAGE;
				break;
			case 150:
				return terrain::MOUNTAIN;
				break;
			case 90:
				return terrain::FOREST;
				break;
			case 30:
				return terrain::REEF;
				break;
			case 39:
				return terrain::BEACH;
				break;

			default:
				break;
		}

		switch((value-300)%10)
		{
			case 0:
				return terrain::HEADQUARTER;
				break;
			case 1:
				return terrain::CITY;
				break;
			case 2:
				return terrain::BASE;
				break;
			case 3:
				return terrain::AIRPORT;
				break;
			case 4:
				return terrain::PORT;
				break;
			case 5:
				return terrain::TOWER;
				break;
			case 6:
				return terrain::LAB;
				break;
		}

		throw std::runtime_error("Undefinierter Wert aus der Karte eingelesen, Abbruch");
	}

	 terrain::extras map_loader::convert_to_terrain_extra(uint16_t value)
	{
		switch((value-300)/10)
		{
			case 0:
				return terrain::RED;
				break;
			case 1:
				return terrain::BLUE;
				break;
			case 2:
				return terrain::GREEN;
				break;
			case 3:
				return terrain::YELLOW;
				break;
			case 4:
				if(m_header.substr(0, 3) == "AWM") //In .awm-files means 4 a white building
					return terrain::WHITE;
				else
					return terrain::BLACK;
				break;
			case 5:
				return terrain::WHITE;
				break;

			default:
				return terrain::NONE;
		}
	}

	 unit::types map_loader::convert_to_unit_type(uint16_t value)
	{
		static std::map<int, std::string> pairs;

		if(pairs.empty())
		{
			BOOST_FOREACH(const aw::unit_loader::value_type &pair, aw::unit_list())
			{
				const unit_loader::loaded_unit &loaded = pair.second;
				pairs.insert(std::make_pair(loaded.id, loaded.internal_name));
			}
		}

		std::map<int, std::string>::iterator it = pairs.find((value-500)%40);

		if(it == pairs.end())
			throw std::runtime_error("[map_loader::convert_to_unit_type] Unit with id " + boost::lexical_cast<std::string>((value-500)%40) + " not in the database");
		else
			return it->second;
	}

	 unit::colors map_loader::convert_to_unit_color(uint16_t value)
	{

		switch((value-500)/40)
		{
			case 0:
				return unit::RED;
				break;
			case 1:
				return unit::BLUE;
				break;
			case 2:
				return unit::GREEN;
				break;
			case 3:
				return unit::YELLOW;
				break;
			case 4:
				return unit::BLACK;
				break;

			default:
				throw std::runtime_error("Unbekannte Einheiten-Farbe eingelesen, Abbruch.");
		}
	}
}

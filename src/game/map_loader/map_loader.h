#ifndef AW_MAP_LOADER_H
	#define AW_MAP_LOADER_H

#include <boost/multi_array.hpp>
#include "game/units/unit_base.h"
#include "game/terrain.h"

namespace aw
{
	class map_loader
	{
		public:
			enum map_type { AWS, AWD, AW2, AWM };

			struct loaded_map
			{
				typedef boost::shared_ptr<loaded_map> ptr;

				loaded_map()
				: terrain(boost::extents[30][20]), unit(boost::extents[30][20])
				{
					for(std::size_t i = 0; i < 30; i++)
						for(std::size_t j = 0; j < 20; j++)
							terrain[i][j] = terrain::create();
				}

				boost::multi_array<terrain::ptr, 2> terrain;
				boost::multi_array<unit::ptr, 2> unit;

			  std::string title;
			  std::string author;
			  std::string description;
			};

			map_loader(const std::string &filename = "");
//			display::scene::ptr load(const std::string &filename = "");
			loaded_map::ptr load(const std::string &filename = "");

			std::string get_name() { return m_name; }
			std::string get_description() { return m_description; }
			std::string get_author() { return m_author; }

			int get_tileset() { return static_cast<int>(m_tileset); }

		private:
			std::size_t read_header();

			std::size_t load_aws(std::size_t position);
			std::size_t load_awd(std::size_t position);
			std::size_t load_awm(std::size_t position); //Also used for .aw2

			void read_metadata(std::size_t position);

			void process_data(uint16_t data, unsigned int x, unsigned int y);

			terrain::types convert_to_terrain_type(uint16_t value);
			terrain::extras convert_to_terrain_extra(uint16_t value);

			unit::types convert_to_unit_type(uint16_t value);
			unit::colors convert_to_unit_color(uint16_t value);

//			display::scene::ptr m_scene;
			loaded_map::ptr m_map;

			std::vector<char> m_data;

			std::string m_filename;

			map_type m_map_type;
			std::string m_header;
			uint8_t m_tileset;
			uint8_t m_size_x;
			uint8_t m_size_y;

			std::string m_name;
			std::string m_author;
			std::string m_description;
	};
}

#endif


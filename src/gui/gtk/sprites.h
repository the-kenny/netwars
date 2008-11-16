#ifndef AW_SPRITE_H
	#define AW_SPRITE_H

#include <cairomm/cairomm.h>
#include <map>

#include <iostream>

#include <boost/filesystem.hpp>

#include "game/utility.h"

namespace aw
{
	namespace sprites
	{
		//TODO: Make abstract
		typedef Cairo::RefPtr<Cairo::ImageSurface> ptr;

		class sprite_manager: public utility::singleton<sprite_manager>
		{
			public:
				const sprites::ptr get_sprite(const std::string &filename)
				{
					if(filename.substr(filename.size()-4) != ".png")
						throw std::runtime_error("File " + filename + " is not a .png");

					std::map<std::string, sprites::ptr>::iterator it = m_sprites.find(filename);

					if(it == m_sprites.end()) //Nicht in der Liste
					{
						if(!boost::filesystem::exists(filename))
							throw std::runtime_error("File not found: " + filename);

//						std::cout << "Lade in den Cache: " << filename << std::endl;
						sprites::ptr p = Cairo::ImageSurface::create_from_png(filename);
						m_sprites[filename] = p;
						return p;
					}
					else
					{
						return it->second;
					}
				}

				void reset() { m_sprites.clear(); }

			private:
				std::map<std::string, sprites::ptr> m_sprites;
		};
	}

	inline sprites::sprite_manager &sprite() { return sprites::sprite_manager::instance(); }
}

#endif

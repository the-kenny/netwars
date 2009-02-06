#include "drawing.h"

#include "paths.h"

namespace aw 
{
	namespace gui 
	{
		std::string get_sprite_for(const coord& c, const scene::ptr& scene) {
			int x = c.x;
			int y = c.y;

			const terrain::types type = scene->get_terrain(x, y)->type();
			const terrain::extras extra = scene->get_terrain(x, y)->extra();

			if(type == terrain::WATER)
			{
				if(y > 0 && x > 0 && terrain::is_ground(scene->get_terrain(x-1, y-1)->type()))
				return get_path(terrain::SEASIDE, terrain::UL, terrain::CORNER); //Ecke oben Links
				if(y < 19 && x > 0 && terrain::is_ground(scene->get_terrain(x-1, y+1)->type()))
				return get_path(terrain::SEASIDE, terrain::LD, terrain::CORNER); //Ecke unten links
				if(y > 0 && x < 29 && terrain::is_ground(scene->get_terrain(x+1, y-1)->type()))
				return get_path(terrain::SEASIDE, terrain::UR, terrain::CORNER); //Ecke oben rechts
				if(y < 19 && x < 29 && terrain::is_ground(scene->get_terrain(x+1, y+1)->type()))
				return get_path(terrain::SEASIDE, terrain::DR, terrain::CORNER); //Ecke unten rechts

				if(x > 0 && terrain::is_ground(scene->get_terrain(x-1, y)->type()))
				return get_path(terrain::SEASIDE, terrain::L); //Küste links
				if(x < 29 && terrain::is_ground(scene->get_terrain(x+1, y)->type()))
				return get_path(terrain::SEASIDE, terrain::R); //Küste rechts
				if(y > 0 && terrain::is_ground(scene->get_terrain(x, y-1)->type()))
				return get_path(terrain::SEASIDE, terrain::U); //Küste oben
				if(y < 19 && terrain::is_ground(scene->get_terrain(x, y+1)->type()))
				return get_path(terrain::SEASIDE, terrain::D); //Küste unten

				if(x > 0 && y < 19 && y > 0 && scene->get_terrain(x-1, y)->type() == terrain::RIVER &&
				(scene->get_terrain(x-1, y+1)->type() != terrain::RIVER && scene->get_terrain(x-1, y-1)->type() != terrain::RIVER))
				return get_path(terrain::RIVER, terrain::L, terrain::MOUTH); //Flussmündung links

				if(x < 29 && y < 19 && y > 0 && scene->get_terrain(x+1, y)->type() == terrain::RIVER &&
				(scene->get_terrain(x+1, y-1)->type() != terrain::RIVER && scene->get_terrain(x+1, y+1)->type() != terrain::RIVER))
				return get_path(terrain::RIVER, terrain::R, terrain::MOUTH); //Flussmündung rechts

				if(y > 0 && x < 29 && x > 0 && scene->get_terrain(x, y-1) ->type()== terrain::RIVER &&
				(scene->get_terrain(x-1, y-1)->type() != terrain::RIVER && scene->get_terrain(x+1, y-1)->type() != terrain::RIVER))
				return get_path(terrain::RIVER, terrain::U, terrain::MOUTH); //Flussmündung oben

				if(y < 19 && x < 29 && x > 0 && scene->get_terrain(x, y+1)->type() == terrain::RIVER &&
				(scene->get_terrain(x-1, y+1)->type() != terrain::RIVER && scene->get_terrain(x+1, y+1)->type() != terrain::RIVER))
				return get_path(terrain::RIVER, terrain::D, terrain::MOUTH); //Flussmündung unten

				if(x > 0 && terrain::is_ground(scene->get_terrain(x-1, y)->type()))
				{
					if(y > 0 && terrain::is_ground(scene->get_terrain(x, y-1)->type()))
					return get_path(terrain::SEASIDE, terrain::UL); //Nach innen gebogene Küste (terrain::UL)

					if(y < 19 && terrain::is_ground(scene->get_terrain(x, y+1)->type()))
					return get_path(terrain::SEASIDE, terrain::LD); //Nach innen gebogene Küste (LD)
				}

				if(x < 29 && terrain::is_ground(scene->get_terrain(x+1, y)->type()))
				{
					if(y > 0 && terrain::is_ground(scene->get_terrain(x, y-1)->type()))
					return get_path(terrain::SEASIDE, terrain::UR); //Nach innen gebogene Küste (UR)
					else if(y < 19 && terrain::is_ground(scene->get_terrain(x, y+1)->type()))
					return get_path(terrain::SEASIDE, terrain::DR); //Nach innen gebogene Küste (DR)
				}

				if(x > 0 && x < 29 && terrain::is_ground(scene->get_terrain(x-1, y)->type()) && terrain::is_ground(scene->get_terrain(x+1, y)->type()))
				{
					if(y > 0 && terrain::is_ground(scene->get_terrain(x, y-1)->type()))
					return get_path(terrain::SEASIDE, terrain::ULR); //Bucht nach unten offen
					else if(y < 19 && terrain::is_ground(scene->get_terrain(x, y+1)->type()))
					return get_path(terrain::SEASIDE, terrain::LDR); //Bucht nach oben offen
				}

				if(y > 0 && y < 19 && terrain::is_ground(scene->get_terrain(x, y-1)->type()) && terrain::is_ground(scene->get_terrain(x, y+1)->type()))
				{
					if(x > 0 && terrain::is_ground(scene->get_terrain(x-1, y)->type()))
					return get_path(terrain::SEASIDE, terrain::ULD); //Bucht nach rechts offen
					else if(x < 29 && terrain::is_ground(scene->get_terrain(x+1, y)->type()))
					return get_path(terrain::SEASIDE, terrain::UDR); //Bucht nach links offen
				}

				if(y > 0 && y < 19 && x > 0 && x < 29)
				{
					if(terrain::is_ground(scene->get_terrain(x, y-1)->type()) && //Oben
					terrain::is_ground(scene->get_terrain(x, y+1)->type()) && //Unten
					terrain::is_ground(scene->get_terrain(x+1, y)->type()) && //Rechts
					terrain::is_ground(scene->get_terrain(x-1, y)->type())) //Links
					return get_path(terrain::SEASIDE, terrain::ULDR); //See (oder was das sein soll)
				}
			}
			else if(type == terrain::RIVER)
			{
				if(y > 0 && y < 19 && x > 0 && x < 29 &&
				is_connectable(terrain::RIVER, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::RIVER, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::RIVER, scene->get_terrain(x+1, y)->type()) && //Rechts
				is_connectable(terrain::RIVER, scene->get_terrain(x-1, y)->type())) //Links)
				{
					return get_path(terrain::RIVER, terrain::ULDR); //Flusskreuzung (ULDR)
				}
				else if(y > 0 && y < 19 && x > 0 &&
				is_connectable(terrain::RIVER, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::RIVER, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::RIVER, scene->get_terrain(x-1, y)->type())) //Links
				{
					return get_path(terrain::RIVER, terrain::ULD); //Fluss mündet in anderen Fluss (terrain::ULD)
				}
				else if(y > 0 && y < 19 && x < 29 &&
				is_connectable(terrain::RIVER, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::RIVER, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::RIVER, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::RIVER, terrain::UDR); //Fluss mündet in anderen Fluss (UDR)
				}
				else if(y < 19 && x > 0 && x < 29 &&
				is_connectable(terrain::RIVER, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::RIVER, scene->get_terrain(x-1, y)->type()) &&//Links
				is_connectable(terrain::RIVER, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::RIVER, terrain::LDR); //Fluss mündet in anderen Fluss (LDR)
				}
				else if(y > 0 && x > 0 && x < 29 &&
				is_connectable(terrain::RIVER, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::RIVER, scene->get_terrain(x-1, y)->type()) &&//Links
				is_connectable(terrain::RIVER, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::RIVER, terrain::ULR); //Fluss mündet in anderen Fluss (ULR)
				}
				else if(y > 0 && x > 0 &&
				is_connectable(terrain::RIVER, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::RIVER, scene->get_terrain(x-1, y)->type())) //Links
				{
					return get_path(terrain::RIVER, terrain::UL); //Kurve (terrain::UL)
				}
				else if(y < 19 && x > 0 &&
				is_connectable(terrain::RIVER, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::RIVER, scene->get_terrain(x-1, y)->type())) //Links
				{
					return get_path(terrain::RIVER, terrain::LD); //Kurve (LD)
				}
				else if(y > 0 && x < 29 &&
				is_connectable(terrain::RIVER, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::RIVER, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::RIVER, terrain::UR); //Kurve (UR)
				}
				else if(y < 19 && x < 29 &&
				is_connectable(terrain::RIVER, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::RIVER, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::RIVER, terrain::DR); //Kurve (DR)
				}
				else if((y > 0 && is_connectable(terrain::RIVER, scene->get_terrain(x, y-1)->type())) || //Oben
				(y < 19 && is_connectable(terrain::RIVER, scene->get_terrain(x, y+1)->type())))
				{
					return get_path(terrain::RIVER, terrain::UD); //Gerade (UD)
				}
				else if((x > 0 && is_connectable(terrain::RIVER, scene->get_terrain(x-1, y)->type())) ||
				(x < 29 && is_connectable(terrain::RIVER, scene->get_terrain(x+1, y)->type())))
				{
					return get_path(terrain::RIVER, terrain::LR); //Gerade (UD)
				}
				else if((y > 0 && scene->get_terrain(x, y-1)->type() == terrain::WATER) ||
				(y < 19 && scene->get_terrain(x, y+1)->type() == terrain::WATER)) //Um einzelne Flusstücke mit Flussmündungen zu verbinden
				{
					return get_path(terrain::RIVER, terrain::UD); //Gerade (UD)
				}
				else if((x > 0 && scene->get_terrain(x-1, y)->type() == terrain::WATER) ||
				(x < 29 && scene->get_terrain(x+1, y)->type() == terrain::WATER)) //Um einzelne Flusstücke mit Flussmündungen zu verbinden
				{
					return get_path(terrain::RIVER, terrain::LR); //Gerade (UD)
				}
			}
			else if(type == terrain::STREET)
			{
				if(y > 0 && y < 19 && x > 0 && x < 29 &&
				is_connectable(terrain::STREET, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::STREET, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::STREET, scene->get_terrain(x+1, y)->type()) && //Rechts
				is_connectable(terrain::STREET, scene->get_terrain(x-1, y)->type())) //Links)
				{
					return get_path(terrain::STREET, terrain::ULDR); //Kreuzung (ULDR)
				}
				else if(y > 0 && y < 19 && x > 0 &&
				is_connectable(terrain::STREET, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::STREET, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::STREET, scene->get_terrain(x-1, y)->type())) //Links
				{
					return get_path(terrain::STREET, terrain::ULD); //Strasse trifft auf querstrasse (terrain::ULD)
				}
				else if(y > 0 && y < 19 && x < 29 &&
				is_connectable(terrain::STREET, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::STREET, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::STREET, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::STREET, terrain::UDR); //Strasse trifft auf querstrasse (UDR)
				}
				else if(y < 19 && x > 0 && x < 29 &&
				is_connectable(terrain::STREET, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::STREET, scene->get_terrain(x-1, y)->type()) &&//Links
				is_connectable(terrain::STREET, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::STREET, terrain::LDR); //Strasse trifft auf querstrasse (LDR)
				}
				else if(y > 0 && x > 0 && x < 29 &&
				is_connectable(terrain::STREET, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::STREET, scene->get_terrain(x-1, y)->type()) &&//Links
				is_connectable(terrain::STREET, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::STREET, terrain::ULR); //Strasse trifft auf querstrasse (ULR)
				}
				else if(y > 0 && x > 0 &&
				is_connectable(terrain::STREET, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::STREET, scene->get_terrain(x-1, y)->type())) //Links
				{
					return get_path(terrain::STREET, terrain::UL); //Kurve (terrain::UL)
				}
				else if(y < 19 && x > 0 &&
				is_connectable(terrain::STREET, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::STREET, scene->get_terrain(x-1, y)->type())) //Links
				{
					return get_path(terrain::STREET, terrain::LD); //Kurve (LD)
				}
				else if(y > 0 && x < 29 &&
				is_connectable(terrain::STREET, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::STREET, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::STREET, terrain::UR); //Kurve (UR)
				}
				else if(y < 19 && x < 29 &&
				is_connectable(terrain::STREET, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::STREET, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::STREET, terrain::DR); //Kurve (DR)
				}
				else if((y > 0 && is_connectable(terrain::STREET, scene->get_terrain(x, y-1)->type())) || //Oben
				(y < 19 && is_connectable(terrain::STREET, scene->get_terrain(x, y+1)->type()))) //unten
				{
					return get_path(terrain::STREET, terrain::UD); //Gerade (UD)
				}
				else if((x > 0 && is_connectable(terrain::STREET, scene->get_terrain(x-1, y)->type())) || //Links
				(x < 29 && is_connectable(terrain::STREET, scene->get_terrain(x+1, y)->type()))) //Rechts
				{
					return get_path(terrain::STREET, terrain::LR); //Gerade (UD)
				}
				else
				{
					return get_path(terrain::STREET, terrain::LR); //Gerade (LR)
				}
			}
			else if(type == terrain::BRIDGE)
			{
				if((y > 0 && is_connectable(terrain::BRIDGE, scene->get_terrain(x, y-1)->type())) || //Oben
				(y < 19 && is_connectable(terrain::BRIDGE, scene->get_terrain(x, y+1)->type()))) //unten
				{
					return get_path(terrain::BRIDGE, terrain::UD); //Gerade (UD)
				}
				else if((x > 0 && is_connectable(terrain::BRIDGE, scene->get_terrain(x-1, y)->type())) || //Links
				(x < 29 && is_connectable(terrain::BRIDGE, scene->get_terrain(x+1, y)->type()))) //Rechts
				{
					return get_path(terrain::BRIDGE, terrain::LR); //Gerade (LR)
				}
			}
			else if(type == terrain::PIPE)
			{
				if(y > 0 && x > 0 &&
				is_connectable(terrain::PIPE, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::PIPE, scene->get_terrain(x-1, y)->type())) //Links
				{
					return get_path(terrain::PIPE, terrain::UL); //Kurve (terrain::UL)
				}
				else if(y < 19 && x > 0 &&
				is_connectable(terrain::PIPE, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::PIPE, scene->get_terrain(x-1, y)->type())) //Links
				{
					return get_path(terrain::PIPE, terrain::LD); //Kurve (LD)
				}
				else if(y > 0 && x < 29 &&
				is_connectable(terrain::PIPE, scene->get_terrain(x, y-1)->type()) && //Oben
				is_connectable(terrain::PIPE, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::PIPE, terrain::UR); //Kurve (UR)
				}
				else if(y < 19 && x < 29 &&
				is_connectable(terrain::PIPE, scene->get_terrain(x, y+1)->type()) && //Unten
				is_connectable(terrain::PIPE, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::PIPE, terrain::DR); //Kurve (DR)
				}
				else if(x > 0 && is_connectable(terrain::PIPE, scene->get_terrain(x-1, y)->type()) && //Links
				x < 29 && is_connectable(terrain::PIPE, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::PIPE, terrain::LR); //Gerade (LR)
				}
				else if(y > 0 && is_connectable(terrain::PIPE, scene->get_terrain(x, y-1)->type()) && //Oben
				y < 19 && is_connectable(terrain::PIPE, scene->get_terrain(x, y+1)->type())) //unten
				{
					return get_path(terrain::PIPE, terrain::UD); //Gerade (UD)
				}
				else if(y > 0 && is_connectable(terrain::PIPE, scene->get_terrain(x, y-1)->type())) //Oben
				{
					return get_path(terrain::PIPE, terrain::U); //Endstück (terrain::U)
				}
				else if(y < 19 && is_connectable(terrain::PIPE, scene->get_terrain(x, y+1)->type())) //Unten
				{
					return get_path(terrain::PIPE, terrain::D); //Endstück (terrain::D)
				}
				else if(x > 0 && is_connectable(terrain::PIPE, scene->get_terrain(x-1, y)->type())) //Links
				{
					return get_path(terrain::PIPE, terrain::L); //Endstück (terrain::L)
				}
				else if(x < 29 && is_connectable(terrain::PIPE, scene->get_terrain(x+1, y)->type())) //Links
				{
					return get_path(terrain::PIPE, terrain::R); //Endstück (terrain::R)
				}
				else
				{
					return get_path(terrain::PIPE, terrain::LR); //Wenn keine Verbindungen möglich sind (LR)
				}
			}
			else if(type == terrain::SEGMENT_PIPE)
			{
				if(x > 0 && is_connectable(terrain::SEGMENT_PIPE, scene->get_terrain(x-1, y)->type()) && //Links
				x < 29 && is_connectable(terrain::SEGMENT_PIPE, scene->get_terrain(x+1, y)->type())) //Rechts
				{
					return get_path(terrain::SEGMENT_PIPE, terrain::LR); //Gerade (LR)
				}
				else if(y > 0 && is_connectable(terrain::SEGMENT_PIPE, scene->get_terrain(x, y-1)->type()) && //Oben
				y < 19 && is_connectable(terrain::SEGMENT_PIPE, scene->get_terrain(x, y+1)->type())) //unten
				{
					return get_path(terrain::SEGMENT_PIPE, terrain::UD); //Gerade (UD)
				}
			}
			else if(type == terrain::WRECKAGE)
			{
				if((y > 0 && is_connectable(type, scene->get_terrain(x, y-1)->type())) ||
				(y < 19 && is_connectable(type, scene->get_terrain(x, y+1)->type())))
				{
					return get_path(terrain::WRECKAGE, terrain::UD);
				}
				else if((x > 0 && is_connectable(type, scene->get_terrain(x-1, y)->type())) ||
				(x < 29 && is_connectable(type, scene->get_terrain(x+1, y)->type())))
				{
					return get_path(terrain::WRECKAGE, terrain::LR);
				}
				else
				{
					//Trümmer
					return get_path(terrain::WRECKAGE, terrain::UNDEF);
				}
			}
			else if(type == terrain::MOUNTAIN)
			{
				if(y > 0)
				{
					return get_path(terrain::MOUNTAIN, terrain::UNDEF, terrain::BIG);
				}
				else
				{
					return get_path(terrain::MOUNTAIN, terrain::UNDEF, terrain::SMALL);
				}
			}
			else if(type == terrain::BEACH)
			{

				if(x > 0 && terrain::is_ground(scene->get_terrain(x-1, y)->type()))
				return get_path(terrain::BEACH, terrain::L); //Küste links
				if(x < 29 && terrain::is_ground(scene->get_terrain(x+1, y)->type()))
				return get_path(terrain::BEACH, terrain::R); //Küste rechts
				if(y > 0 && terrain::is_ground(scene->get_terrain(x, y-1)->type()))
				return get_path(terrain::BEACH, terrain::U); //Küste oben
				if(y < 19 && terrain::is_ground(scene->get_terrain(x, y+1)->type()))
				return get_path(terrain::BEACH, terrain::D); //Küste unten


				if(x > 0 && terrain::is_ground(scene->get_terrain(x-1, y)->type()))
				{
					if(y > 0 && terrain::is_ground(scene->get_terrain(x, y-1)->type()))
					return get_path(terrain::BEACH, terrain::UL); //Nach innen gebogene Küste (terrain::UL)

					if(y < 19 && terrain::is_ground(scene->get_terrain(x, y+1)->type()))
					return get_path(terrain::BEACH, terrain::LD); //Nach innen gebogene Küste (LD)
				}

				if(x < 29 && terrain::is_ground(scene->get_terrain(x+1, y)->type()))
				{
					if(y > 0 && terrain::is_ground(scene->get_terrain(x, y-1)->type()))
					return get_path(terrain::BEACH, terrain::UR); //Nach innen gebogene Küste (UR)

					if(y < 19 && terrain::is_ground(scene->get_terrain(x, y+1)->type()))
					return get_path(terrain::BEACH, terrain::DR); //Nach innen gebogene Küste (DR)
				}

				if(y > 0 && y < 19 &&
				terrain::is_ground(scene->get_terrain(x, y-1)->type()) &&
				terrain::is_ground(scene->get_terrain(x, y+1)->type()))
				{
					if(x > 0 && terrain::is_ground(scene->get_terrain(x-1, y)->type()))
					return get_path(terrain::BEACH, terrain::ULD); //Bucht mit Strand
					else if(x < 29 && terrain::is_ground(scene->get_terrain(x+1, y)->type()))
					return get_path(terrain::BEACH, terrain::UDR); //Bucht mit Strand
				}

				if(x > 0 && x < 29 &&
				terrain::is_ground(scene->get_terrain(x-1, y)->type()) &&
				terrain::is_ground(scene->get_terrain(x+1, y)->type()))
				{
					if(y > 0 && terrain::is_ground(scene->get_terrain(x, y-1)->type()))
					return get_path(terrain::BEACH, terrain::ULR); //Bucht mit Strand
					else if(y < 19 && terrain::is_ground(scene->get_terrain(x, y+1)->type()))
					return get_path(terrain::BEACH, terrain::LDR); //Bucht mit Strand
				}
			}
			else if(type != terrain::WATER)
			{
				return get_path(scene->get_terrain(x, y)->type(), terrain::UNDEF, extra);
			}

			return "";
		}

		bool is_connectable(terrain::types lhs, terrain::types rhs)
		{
			switch(lhs)
			{
				case terrain::STREET:
				if(rhs == terrain::STREET || rhs == terrain::BRIDGE)
				return true;
				break;
				case terrain::PIPE:
				case terrain::SEGMENT_PIPE:
				if(rhs == terrain::PIPE || rhs == terrain::SEGMENT_PIPE || rhs == terrain::WRECKAGE || rhs == terrain::BASE)
				return true;
				break;
				case terrain::WRECKAGE:
				if(rhs == terrain::PIPE || rhs == terrain::SEGMENT_PIPE)
				return true;
				break;
				case terrain::BRIDGE:
				if(rhs == terrain::BRIDGE || (terrain::is_ground(rhs) && rhs != terrain::RIVER))
				return true;
				break;
				case terrain::RIVER:
				if(rhs == terrain::RIVER || rhs == terrain::BRIDGE)
				return true;
				break;

				default:
				break;
			}

			return false;
		}
	}
}

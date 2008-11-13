#include "paths.h"

#include "../config.h"
#include "../units/unit_loader.h"

using namespace aw;

static const std::string base_path(aw::config().get<std::string>("/config/dirs/pixmaps"));

std::string display::get_path(terrain::types type, terrain::directions direction, terrain::extras extra)
{
//	static std::string base_path(aw::config().get<std::string>("/config/dirs/pixmaps") + "ground/");
	std::string path(base_path + "ground/");

	switch(type)
	{
		case terrain::PLAIN:
			return path + "misc/plain.png";
			break;
		case terrain::WATER:
			return path + "misc/water.png";
			break;
		case terrain::RIVER:
			path += "river/";
			break;
		case terrain::SEASIDE:
			path += "seaside/";
			break;
		case terrain::STREET:
			path += "street/";
			break;
		case terrain::BRIDGE:
			path += "street/bridge_";
			break;
		case terrain::PIPE:
			path += "pipe/";
			break;
		case terrain::SEGMENT_PIPE:
			path += "pipe/segment_";
			break;
//				case terrain::BROKEN_PIPE:
//					path += "pipe/broken_";
//					break;
		case terrain::WRECKAGE:
			path += "pipe/broken_";
			break;
		case terrain::MOUNTAIN:
			path += "mountain/";
			break;
		case terrain::FOREST:
			return path + "misc/forest.png";
			break;
		case terrain::REEF:
			return path + "misc/reef.png";
			break;
		case terrain::BEACH:
			path += "beach/";
			break;
		case terrain::HEADQUARTER:
			path += "buildings/headquarter/";
			break;
		case terrain::CITY:
			path += "buildings/city/";
			break;
		case terrain::BASE:
			path += "buildings/base/";
			break;
		case terrain::AIRPORT:
			path += "buildings/airport/";
			break;
		case terrain::PORT:
			path += "buildings/port/";
			break;
		case terrain::TOWER:
			path += "buildings/tower/";
			break;
		case terrain::LAB:
			path += "buildings/lab/";
			break;

		default:
			return base_path + "misc/blank.png";
			break;
	}

	switch(extra)
	{
		case terrain::NONE:
			if(terrain::is_building(type))
				throw std::logic_error("Für Gebäude muss eine Farbe definiert werden");
			break;

		case terrain::CORNER:
			if(type != terrain::SEASIDE) throw std::logic_error("Nur auf Küsten können die Ecken angewendet werden!");
			path += "corner_";
			break;
		case terrain::BIG:
			if(type != terrain::MOUNTAIN) throw std::logic_error("Nur Berge können groß oder klein sein!");
			path += "big";
			break;
		case terrain::SMALL:
			if(type != terrain::MOUNTAIN) throw std::logic_error("Nur Berge können groß oder klein sein!");
			path += "small";
			break;
		case terrain::MOUTH:
			if(type != terrain::RIVER) throw std::logic_error("Nur Flüsse können ins Meer fließen!");
			path += "mouth_";
			break;
		case terrain::RED:
			if(!terrain::is_building(type)) throw std::logic_error("Nur Gebäude haben Farben!");
			path += "red";
			break;
		case terrain::BLUE:
			if(!terrain::is_building(type)) throw std::logic_error("Nur Gebäude haben Farben!");
			path += "blue";
			break;
		case terrain::GREEN:
			if(!terrain::is_building(type)) throw std::logic_error("Nur Gebäude haben Farben!");
			path += "green";
			break;
		case terrain::YELLOW:
			if(!terrain::is_building(type)) throw std::logic_error("Nur Gebäude haben Farben!");
			path += "yellow";
			break;
		case terrain::BLACK:
			if(!terrain::is_building(type)) throw std::logic_error("Nur Gebäude haben Farben!");
			path += "black";
			break;
		case terrain::WHITE:
			if(!terrain::is_building(type)) throw std::logic_error("Nur Gebäude haben Farben!");
			path += "white";
			break;
		case terrain::LAUNCHED:
			if(type != terrain::HEADQUARTER) throw std::logic_error("Nur Raketensilog können abgefeuert werden!");
			path += "white_shot";
			break;

		default:
			return base_path + "misc/blank.png";
			break;
	}

	switch(direction)
	{
		case terrain::D:
			path += 'd';
			break;
		case terrain::U:
			path += 'u';
			break;
		case terrain::R:
			path += 'r';
			break;
		case terrain::L:
			path += 'l';
			break;
		case terrain::DR:
			path += "dr";
			break;
		case terrain::LD:
			path += "ld";
			break;
		case terrain::LDR:
			path += "ldr";
			break;
		case terrain::UL:
			path += "ul";
			break;
		case terrain::ULD:
			path += "uld";
			break;
		case terrain::ULDR:
			path += "uldr";
			break;
		case terrain::UD:
			path += "ud";
			break;
		case terrain::ULR:
			path += "ulr";
			break;
		case terrain::UR:
			path += "ur";
			break;
		case terrain::UDR:
			path += "udr";
			break;
		case terrain::LR:
			path += "lr";
			break;

		case terrain::UNDEF:
			if(type == terrain::WRECKAGE)
				path.erase(path.end()-1); // _ entfernen
			break;
	}

	return (path + ".png");
}

std::string display::get_path(unit::types type, unit::colors color)
{
//	static std::string base_path(aw::config().get<std::string>("/config/dirs/pixmaps");
	std::string path(base_path + "units/");

	switch(color)
	{
		case unit::RED:
			path += "os/";
			break;
		case unit::BLUE:
			path += "bm/";
			break;
		case unit::GREEN:
			path += "ge/";
			break;
		case unit::YELLOW:
			path += "yc/";
			break;
		case unit::BLACK:
			path += "bh/";
			break;
		case unit::UNDEF:
			return base_path + "misc/blank.png";
	}

	path += unit_list().get_unit_info(type).filename;
	return path;
}

std::string display::get_path(unit::extras extra, int live)
{
	if(extra == unit::LIVE && (live <= 0 || live > 9))
		throw std::runtime_error("Invalid Arguments");

//	static std::string base_path(aw::config().get<std::string>("/config/dirs/pixmaps", true) + "units/misc/");
	std::string path(base_path + "units/misc/");

	switch(extra)
	{
		case unit::LIVE:
		{
			if(live < 0 || live > 10)
				throw std::runtime_error("Lebenspunkte müssen < 10 & > 0 sein.");
			switch(live)
			{
				case 10:
					return "";
				case 9:
					path += "nine";
					break;
				case 8:
					path += "eight";
					break;
				case 7:
					path += "seven";
					break;
				case 6:
					path += "six";
					break;
				case 5:
					path += "five";
					break;
				case 4:
					path += "four";
					break;
				case 3:
					path += "three";
					break;
				case 2:
					path += "two";
					break;
				case 1:
					path += "one";
					break;
			}
		}
		break;

		case unit::LOW_AMMO:
			path += "ammo";
			break;
		case unit::LOW_FUEL:
			path += "fuel";
			break;
		case unit::HIDDEN:
			path += "hidden";
			break;
		case unit::LOADED:
			path += "loaded";
			break;
		case unit::CAPTURE:
			path += "capture";
			break;
		case unit::MINUS: //minus is used during fow, when the opponent look at your transports
			path += "minus";
			break;
	}

	return path + ".png";
}

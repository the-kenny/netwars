#ifndef AW_UTILITY_H
	#define AW_UTILITY_H

#include <boost/utility.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include "tinyxml/ticpp.h"

namespace aw
{
	namespace utility
	{
		template<class derived>
		class singleton: public boost::noncopyable
		{
			public:
				static derived &instance()
				{
					static derived d;
					return d;
				}
		};

		//NOTE: Use boost.property_tree when it is released
		class xml_reader: public singleton<xml_reader>
		{
			public:
				xml_reader() { }
				xml_reader(const std::string &file)
				{
					if(!file.empty())
						this->load(file);
				}

				void load(const std::string &file)
				{
					m_xml.LoadFile(file);
				}

				template<typename T>
				const T get(const std::string &path)
				{
					const char delim = '/';

					std::vector<std::string> tokens;
					tokens.reserve(std::count(path.begin(), path.end(), delim));

					std::string::size_type last_pos = path.find_first_not_of(delim, 0);
					std::string::size_type pos = path.find_first_of(delim, last_pos);

					while(pos != std::string::npos || last_pos != std::string::npos)
					{
						tokens.push_back(path.substr(last_pos, pos - last_pos));
						last_pos = path.find_first_not_of(delim, pos);
						pos = path.find_first_of(delim, last_pos);
					}

					ticpp::Element *element = m_xml.FirstChildElement(tokens.at(0));
					tokens.erase(tokens.begin());

					BOOST_FOREACH(const std::string &s, tokens)
					{
						element = element->FirstChildElement(s);

						if(element->NextSiblingElement(s, false) != NULL)
							std::clog << "[xml_reader::get] WARNING: There is more than one element with the value '" + s + "'" << std::endl;
					}

					return boost::lexical_cast<T>(element->GetText());
				}

				template<typename T>
				void set(const std::string &path, const T &value);

			private:
				ticpp::Document m_xml;
		};
	}
}

#endif

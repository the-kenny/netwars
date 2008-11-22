#ifndef AW_CONFIG_H
	#define AW_CONFIG_H

#include <string>
#include <map>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/program_options.hpp>
#include "tinyxml/ticpp.h"
#include "utility.h"

namespace aw
{
	class config: public utility::singleton<config>
	{
		public:
			void load(const std::string &conf) { m_xml_reader.load(conf); }

			template<typename T>
			const T get(const std::string &path, bool cache = true)
			{
				std::map<std::string, std::string>::iterator iter = m_cache.find(path);
				if(iter != m_cache.end())
				{
					return boost::lexical_cast<T>(std::string(iter->second));
				}
				else
				{
					std::string ret = m_xml_reader.get<std::string>(path);

					if(ret.find("$datadir") != std::string::npos)
						boost::algorithm::replace_all(ret, "$datadir", this->get<std::string>("/config/dirs/data"));

					if(cache)
					{
						m_cache[path] = ret;
					}

					return boost::lexical_cast<T>(ret);
				}
			}

			//Currently only saves to the cache
			template<typename T>
			void set(const std::string &path, const T &value)
			{
				m_cache.insert(std::make_pair(path, boost::lexical_cast<std::string>(value)));
			}
	
			template<typename T>
			T get_cmd(const std::string &name)
			{
				return boost::lexical_cast<T>(m_cmd_arguments[name]);
			}


		private:
			std::map<std::string, std::string> m_cache;
			utility::xml_reader m_xml_reader;	

			//Command line stuff
			std::map<std::string, std::string> m_cmd_arguments;
	};

	inline aw::config &config() { std::cout << "Bla" << std::endl; return config::instance(); }
}

#endif

#ifndef AW_CONFIG_H
	#define AW_CONFIG_H

#include <string>
#include <map>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "tinyxml/ticpp.h"
#include "utility.h"

namespace aw
{
	class config: public utility::singleton<config>
	{
		public:
			config() { m_xml_reader.load("config.xml"); }

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

			template<typename T>
			void set(const std::string &path, const T &value);

		private:
			std::map<std::string, std::string> m_cache;
			utility::xml_reader m_xml_reader;
	};

	inline aw::config &config() { return config::instance(); }
}

#endif

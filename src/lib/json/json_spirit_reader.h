#ifndef JASON_SPIRIT_READER
#define JASON_SPIRIT_READER

/* Copyright (c) 2007-2008 John W Wilkinson

   json spirit version 2.06

   This source code can be used for any purpose as long as
   this comment is retained. */

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "json_spirit_value.h"
#include <iostream>

namespace json_spirit
{
    // functions to reads a JSON values

    bool read( const std::string&  s, Value&  value, bool utf8 = true );
    bool read( std::istream&  is,     Value&  value, bool utf8 = true );

#ifndef BOOST_NO_STD_WSTRING
    bool read( const std::wstring& s, wValue& value );
    bool read( std::wistream& is,     wValue& value );
#endif
}

#endif

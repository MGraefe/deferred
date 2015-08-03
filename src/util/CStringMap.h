// util/CStringMap.h
// util/CStringMap.h
// util/CStringMap.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__CStringMap_H__
#define deferred__util__CStringMap_H__

#include <map>

template< typename Val >
class CStringMap
{
	CStringMap();
	~CStringMap();


private:
	std::map< std::string, Val > m_map;
};



#endif

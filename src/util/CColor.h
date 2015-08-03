// util/CColor.h
// util/CColor.h
// util/CColor.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__util__CColor_H__
#define deferred__util__CColor_H__

//#include "error.h"

typedef unsigned char BYTE;
typedef unsigned int UINT;

class Color
{
public:
	Color() : cl_int(0) { }
	Color( const Color &c ) : cl_int(c.cl_int) { }
	Color( BYTE R, BYTE G, BYTE B, BYTE A = 255 ) : r(R), g(G), b(B), a(A) 
	{ 

	}

	inline Color &operator=(const Color &c)
	{
		cl_int = c.cl_int;
		return *this;
	}

	inline bool operator==(const Color &c) const
	{
		return cl_int == c.cl_int;
	}

	inline bool operator!=(const Color &c) const
	{
		return cl_int != c.cl_int;
	}


public:
	union
	{ 
		struct{ BYTE r,g,b,a; };
		UINT cl_int; //for easier access to members
	};
};

//extern Color g_clButton;
//extern Color g_clText;
//extern Color g_clWindow;

//#define BUTTON_COLOR g_clButton //136 75 255
//#define TEXT_COLOR g_clText //255 255 255
//#define WINDOW_COLOR g_clWindow //136 75 255

#endif

// studiorpm/unicode.cpp
// studiorpm/unicode.cpp
// studiorpm/unicode.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#include "unicode.h"

void a2w(const std::string &src, std::wstring &dst)
{
	dst.assign(src.length(), L'a');
	for(size_t i = 0; i < src.length(); i++)
		mbtowc(&dst[i], &src[i], 1);
}

void w2a(const std::wstring &src, std::string &dst)
{
	dst.assign(src.length(), 'a');
	int dummy;
	for(size_t i = 0; i < src.length(); i++ )
		wctomb_s(&dummy, &dst[i], 1, src[i]);
}

void a2a(const std::string &src, std::string &dst)
{
	dst = src;
}

void w2w(const std::wstring &src, std::wstring &dst)
{
	dst = src;
}
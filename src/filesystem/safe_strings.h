// filesystem/safe_strings.h
// filesystem/safe_strings.h
// filesystem/safe_strings.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once

#include <algorithm>
#include <cstring>

inline int strcpys(char *dest, const char *source, size_t size)
{
	size_t len = std::min(size, strlen(source)+1);
	memcpy(dest, source, len);
	dest[len-1] = '\0';
	return 0;
}

template<size_t size>
inline int strcpys(char (&dest)[size], const char *source)
{
	return strcpys(dest, source, size);
}

// util/os_specific.h
// util/os_specific.h
// util/os_specific.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__os_specific_H__
#define deferred__util__os_specific_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <exception>
#include <cstdarg>
#include <algorithm>

#ifdef NOINLINE
#error NOINLINE ALREADY DEFINED
#endif

#if defined (_MSC_VER)
	#define NOINLINE __declspec(noinline)
	#define snprintf sprintf_s
#elif defined (__GNUC__)
	#define NOINLINE __attribute__ ((noinline))
#else
	#error UNKNOWN KOMPILER
#endif

#ifndef _WINDOWS
typedef unsigned int DWORD;
#endif

#ifndef _WINDOWS

inline char *itoa(int value, char *str, int base)
{
	if(base != 10)
		throw std::exception();
	sprintf(str, "%i", value);
	return 0;
}

#define _itoa itoa

inline int _itoa_s(int value, char *buffer, size_t sizeInCharacters, int base)
{
	if(base != 10)
		throw std::exception();
	snprintf(buffer, sizeInCharacters, "%i", value);
	return 0;
}

template<size_t size>
inline int _itoa_s(int value, char (&buffer)[size], int base)
{
	return _itoa_s(value, buffer, size, base);
}

inline int strcpy_s(char *dest, size_t size, const char *source)
{
	strncpy(dest, source, size);
	dest[size-1] = '\0';
	return 0;
}

template<size_t size>
inline int strcpy_s(char (&dest)[size], const char *source)
{
	return strcpy_s(dest, size, source);
}

inline int vsprintf_s(char *dest, size_t size, const char *format, va_list args)
{
	return vsnprintf(dest, size, format, args);
}

template<size_t size>
inline int vsprintf_s(char (&dest)[size], const char *format, va_list args)
{
	return vsprintf_s(dest, size, format, args);
}

inline int strcat_s(char *dest, size_t size, const char *source)
{
	strncat(dest, source, size-strlen(dest));
	dest[size-1] = '\0';
	return 0;
}

template<size_t size>
inline int strcat_s(char (&dest)[size], const char *source)
{
	return strcat_s(dest, size, source);
}

template<size_t size>
inline int sprintf_s(char (&dest)[size], const char *format, ...)
{
	va_list list;
	va_start(list, format);
	int ret = snprintf(dest, size, format, list);
	va_end(list);
	return ret;
}

template<size_t size>
inline int strncpy_s(char (&dest)[size], const char *source, size_t n)
{
	strncpy(dest, source, std::min(size-1, n));
	return 0;
}


#endif //ifndef _WINDOWS

#endif /* OS_SPECIFIC_H_ */

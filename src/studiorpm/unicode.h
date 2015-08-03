// studiorpm/unicode.h
// studiorpm/unicode.h
// studiorpm/unicode.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__studiorpm__unicode_H__
#define deferred__studiorpm__unicode_H__

#include <string>

void a2w(const std::string &src, std::wstring &dst);
void w2a(const std::wstring &src, std::string &dst);
void a2a(const std::string &src, std::string &dst);
void w2w(const std::wstring &src, std::wstring &dst);

#ifdef UNICODE
#define a2t a2w
#define w2t w2w 
#define t2a w2a
#define t2w w2w
#else
#define a2t a2a
#define w2t w2a 
#define t2a a2a
#define t2w a2w
#endif

#endif // deferred__studiorpm__unicode_H__
// client/ILocaleStringReceiver.h
// client/ILocaleStringReceiver.h
//ILocaleStringReceiver.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__client__ILocaleStringReceiver_H__
#define deferred__client__ILocaleStringReceiver_H__

#include <string>

class ILocaleStringReceiver
{
public:
	ILocaleStringReceiver();
	virtual ~ILocaleStringReceiver();
	virtual void SetText(const std::string &text) = 0;
	virtual const std::string &GetTextLoc() const = 0;
};

#endif // deferred__client__ILocaleStringReceiver_H__
// server/LapTimeTableEntry.h
// server/LapTimeTableEntry.h
// server/LapTimeTableEntry.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__server__LapTimeTableEntry_H__
#define deferred__server__LapTimeTableEntry_H__

class LapTimeTableEntry
{
public:
	LapTimeTableEntry() :
		clientId(-1),
		time(-1.0f)
	{}

	int clientId;
	float time;
};


#define LAPTIMETABLE_NUM_ENTRIES 32

#endif // deferred__server__LapTimeTableEntry_H__
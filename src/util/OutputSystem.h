// util/OutputSystem.h
// util/OutputSystem.h
// util/OutputSystem.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__OutputSystem_H__
#define deferred__util__OutputSystem_H__

#include <queue>
#include <util/datadesc.h>

class OutputSystem
{
public:
	enum genericTarget_e
	{
		TARGET_ERROR = -1,
		TARGET_SELF = 0,
		TARGET_ACTIVATOR,
		TARGET_CALLER,

		TARGET_COUNT //has to be the last one
	};

	inline static const TCHAR *getGenericTargetName(genericTarget_e t)
	{
		switch(t)
		{
			case TARGET_SELF:		return _T("!self");
			case TARGET_ACTIVATOR:	return _T("!activator");
			case TARGET_CALLER:		return _T("!caller");
			default:
				AssertMsg(false, "Added a new generic-output-target?");
				return NULL;
		}
	}

	inline static bool isGenericTargetName(const TCHAR *target1, genericTarget_e target2)
	{
		return _tcscmp(getGenericTargetName((genericTarget_e)target2), target1) == 0;
	}

	inline static genericTarget_e getGenericTarget(const TCHAR *target)
	{
		for(int i = 0; i < TARGET_COUNT; i++)
			if( isGenericTargetName(target, (genericTarget_e)i) )
				return (genericTarget_e)i;
		return TARGET_ERROR;
	}

	inline static bool isGenericTargetName(const TCHAR *target)
	{
		return getGenericTarget(target) != TARGET_ERROR;
	}
};


#endif // deferred__util__OutputSystem_H__

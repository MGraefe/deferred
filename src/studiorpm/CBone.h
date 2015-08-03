// studiorpm/CBone.h
// studiorpm/CBone.h
// studiorpm/CBone.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__studiorpm__CBone_H__
#define deferred__studiorpm__CBone_H__

#include <string>

class CBone
{
public:
	CBone(const std::string &boneName, int boneId, int boneParentId)
		: name(boneName), id(boneId), parentId(boneParentId) { }
	const std::string &getName() { return name; }
	int getId() { return id; }
	int getParentId() { return parentId; }
private:
	std::string name;
	int id;
	int parentId;
};

#endif // deferred__studiorpm__CBone_H__
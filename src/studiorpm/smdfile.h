// studiorpm/smdfile.h
// studiorpm/smdfile.h
// studiorpm/smdfile.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once
#ifndef deferred__studiorpm__smdfile_H__
#define deferred__studiorpm__smdfile_H__

#include <vector>
#include <string>
#include "CBone.h"
#include "CInputFile.h"

class CSMDInputFile : public CInputFile
{
public:
	virtual error_e parse(std::string filename);
	virtual std::vector<rpm_triangle_t> &triangles();
	virtual std::vector<int> parentBones();
	virtual std::vector<std::string> &textures();
	virtual std::vector<CBone> &bones();

private:
	void clear();
	std::vector<rpm_triangle_t> m_triangles;
	std::vector<std::string> m_textures;
	std::vector<int> m_parentBones;
	std::vector<CBone> m_bones;

};




//namespace smd
//{
//	char* parseTriangle(char *start, std::vector<rpm_triangle_t> &tris, int texId, std::vector<int> &parentBones);
//	error_e parseTriangleBlock(char *start, std::vector<rpm_triangle_t> &tris, std::vector<std::string> &textures, std::vector<int> &parentBones);
//	error_e readBoneList(const char *pchFile, std::vector<CBone> &boneList);
//};


#endif // deferred__studiorpm__smdfile_H__
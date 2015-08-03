// studiorpm/CObjInputFile.h
// studiorpm/CObjInputFile.h
// studiorpm/CObjInputFile.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__studiorpm__CObjInputFile_H__
#define deferred__studiorpm__CObjInputFile_H__

#include "CInputFile.h"
#include <util/maths.h>



class CObjInputFile : public CInputFile
{
public:
	virtual error_e parse(std::string filename);
	virtual std::vector<rpm_triangle_t> &triangles();
	virtual std::vector<int> parentBones();
	virtual std::vector<std::string> &textures();
	virtual std::vector<CBone> &bones();

private:
	struct ObjCorner
	{
		int v, u, n;
	};

	struct ObjFace
	{
		int cornerId;		//index to the corner array
		int cornerCount;	//number of corners (at least 3)
		int groupId;		//group id (later becomes bone id)
		int materialId;		//index into the materials array
	};

private:
	void clear();
	error_e parseMaterialLibs( const std::string &objFilename, const std::vector<std::string> &mtllibs, std::map<std::string, std::string, LessThanCppStringObj> &materialTextureMap );

private:
	std::vector<rpm_triangle_t> m_triangles;
	std::vector<std::string> m_textures;
	std::vector<int> m_parentBones;
	std::vector<CBone> m_bones;
};

#endif // deferred__studiorpm__CObjInputFile_H__
// studiorpm/smdfile.cpp
// studiorpm/smdfile.cpp
// studiorpm/smdfile.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "studiorpm.h"
#include "smdfile.h"
#include "CBone.h"


char* parseTriangle(char *start, std::vector<rpm_triangle_t> &tris, int texId, std::vector<int> &parentBones)
{
	rpm_triangle_t tri;
	tri.iTexID = texId;

	for( int i = 0; i < 3; i++ ) // 3 vertexes
	{
		int mainParent = 0;
		float pos[3] = {0.0f};
		float norm[3] = {0.0f};
		float uv[2] = {0.0f};
		int numRead = 0;
		//sscanf_s(start, "%i%[ \t]%f%[ \t]%f%[ \t]%f%[ \t]%f%[ \t]%f%[ \t]%f%[ \t]%f%[ \t]%f%n",
		sscanf_s(start, "%i %f %f %f %f %f %f %f %f%n",
			&mainParent, &pos[0], &pos[1], &pos[2], &norm[0], &norm[1], &norm[2], &uv[0], &uv[1], &numRead);

		if( numRead > 0 )
		{
			rpm_vertex_t &v = tri.vertex3[i];
			for( int k = 0; k < 3; k++ )
				v.pos3[k] = pos[k];
			for( int k = 0; k < 3; k++ )
				v.norm3[k] = norm[k];
			for( int k = 0; k < 2; k++ )
				v.uv2[k] = uv[k];

			//Try to read additional bone weights
			char *boneWeightStart = start + numRead;
			int links;
			int boneId;
			float weight;
			int numRead = 0;
			int ret = sscanf_s(boneWeightStart, " %i %i %f", &links, &boneId, &weight);
			if( ret == 3 )
				parentBones.push_back(boneId);
			else
				parentBones.push_back(mainParent);
		}

		char *endLn = strchr(start, '\n');
		start = endLn + 1;
	}

	calcTriangleNormal(tri);

	tris.push_back(tri);

	return start;
}


error_e parseTriangleBlock(char *start, std::vector<rpm_triangle_t> &tris, std::vector<std::string> &textures, std::vector<int> &parentBones)
{
	static const char triBlockStart[] = "\ntriangles\n";
	start = strstr( start, triBlockStart );
	if(!start)
		return ERR_CORRUPT_FILE;
	start += strlen(triBlockStart);

	while(true)
	{
		//read texture name
		char texname[256];
		start = CopyUntilOccurenceOfChar(texname, start, "\n");
		if( strcmp(texname, "end") == 0 )
			break;

		//Get texture id
		int texId = getTextureIndex(textures, texname);
		if( texId < 0 )
		{
			textures.push_back(std::string(texname));
			texId = textures.size()-1;
		}

		start = parseTriangle(start, tris, texId, parentBones);
	}

	return ERR_NONE;
}


error_e readBoneList(const char *pchFile, std::vector<CBone> &boneList)
{
	const char *nodeStr = strstr(pchFile, "\nnodes\n");
	if(!nodeStr)
		return ERR_CORRUPT_FILE;
	const char *boneLine = nodeStr + strlen("\nnodes\n");
	while(strncmp(boneLine, "end\n", 4) != NULL)
	{
		char chId[32] = {0};
		int id = -1;
		int parentId = -1;
		char boneName[256] = {0};
		int readCount = 0;
		int ret = sscanf(boneLine, "%s %s %i%n", chId, boneName, &parentId, &readCount);
		if(readCount > 0)
		{
			id = atoi(chId);
			string strBoneName(boneName);
			strBoneName.erase(strBoneName.begin()+strBoneName.size()-1);
			strBoneName.erase(strBoneName.begin());
			boneList.push_back(CBone(strBoneName, id, parentId));
			const char *boneLineEnd = strchr(boneLine + readCount, '\n');
			if(!boneLineEnd)
				return ERR_CORRUPT_FILE;
			boneLine = boneLineEnd + 1;
		}
		else
			return ERR_CORRUPT_FILE;
	}
	return ERR_NONE;
}




//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
error_e CSMDInputFile::parse( std::string filename )
{
	clear();

	std::ifstream is(filename);
	if(!is.is_open() || !is.good())
		return ERR_FILENOTFOUND;

	is.seekg(0, std::ios::end);
	size_t filesize = (size_t)is.tellg();
	is.seekg(0, std::ios::beg);

	char *data = new char[filesize];
	is.read(data, filesize);
	is.close();

	error_e e;
	e = parseTriangleBlock(data, m_triangles, m_textures, m_parentBones);
	if(e != ERR_NONE)
		return e;

	e = readBoneList(data, m_bones);
	if(e != ERR_NONE)
		return e;

	return ERR_NONE;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::vector<rpm_triangle_t> & CSMDInputFile::triangles()
{
	return m_triangles;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::vector<int> CSMDInputFile::parentBones()
{
	return m_parentBones;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::vector<std::string> & CSMDInputFile::textures()
{
	return m_textures;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::vector<CBone> & CSMDInputFile::bones()
{
	return m_bones;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSMDInputFile::clear()
{
	m_bones.clear();
	m_parentBones.clear();
	m_textures.clear();
	m_triangles.clear();
}

// studiorpm/CObjInputFile.cpp
// studiorpm/CObjInputFile.cpp
// studiorpm/CObjInputFile.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "studiorpm.h"
#include "CObjInputFile.h"
#include <fstream>
#include <map>

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
error_e CObjInputFile::parse( std::string filename )
{
	clear();

	std::ifstream is(filename);
	if(!is.is_open())
		return ERR_FILENOTFOUND;

	std::vector<Vector3f> verts;
	std::vector<Vector2f> uvs;
	std::vector<Vector3f> norms;
	std::vector<ObjFace> faces;
	std::vector<std::string> groups;
	std::vector<std::string> materials;
	std::vector<std::string> mtllibs;
	std::map<std::string, std::string, LessThanCppStringObj> materialTextureMap;
	std::vector<ObjCorner> corners;

	int currentMaterial = -1;

	char lineRaw[256] = {0};
	while(!is.getline(lineRaw, sizeof(lineRaw)).fail())
	{
		char *line = lineRaw;
		while(*line == ' ' || *line == '\t') //ignore whitespaces
			line++;

		if(line[0] == 'g' && line[1] == ' ')
		{
			char name[128];
			if(sscanf(line, "g %s", name) != 1)
				return ERR_CORRUPT_FILE;
			groups.push_back(name);
		}
		else if(line[0] == 'v' && line[1] == ' ') //vertex
		{
			Vector3f v;
			if(sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z) != 3)
				return ERR_CORRUPT_FILE;
			verts.push_back(v);
		}
		else if(line[0] == 'v' && line[1] == 't' && line[2] == ' ') //uv coord
		{
			Vector2f uv;
			if(sscanf(line, "vt %f %f %f", &uv.x, &uv.y) != 2)
				return ERR_CORRUPT_FILE;
			uvs.push_back(uv);
		}
		else if(line[0] == 'v' && line[1] == 'n' && line[2] == ' ') //normal
		{
			Vector3f n;
			if(sscanf(line, "vn %f %f %f", &n.x, &n.y, &n.z) != 3)
				return ERR_CORRUPT_FILE;
			norms.push_back(n);
		}
		else if(line[0] == 'f' && line[1] == ' ')
		{
			char *scan = line + 2;
			ObjFace face;
			face.groupId = (int)groups.size() - 1;
			face.materialId = currentMaterial;
			face.cornerId = corners.size();
			face.cornerCount = 0;
			ObjCorner c;
			int num = 0;
			while(sscanf(scan, "%i/%i/%i%n", &c.v, &c.u, &c.n, &num) == 3)
			{
				corners.push_back(c);
				face.cornerCount++;
				scan += num;
			}
			if(face.cornerCount < 3)
				return ERR_CORRUPT_FILE;				
			faces.push_back(face);
		}
		else if(strncmp(line, "usemtl ", 7) == 0)
		{
			char name[128];
			if(sscanf(line, "usemtl %s", name) != 1)
				return ERR_CORRUPT_FILE;
			auto it = std::find(materials.begin(), materials.end(), name);
			if(it == materials.end())
			{
				materials.push_back(name);
				currentMaterial = materials.size() - 1;
			}
			else
				currentMaterial = (int)(it - materials.begin());
		}
		else if(strncmp(line, "mtllib ", 7) == 0)
		{
			char name[128];
			if(sscanf(line, "mtllib %s", name) != 1)
				return ERR_CORRUPT_FILE;
			mtllibs.push_back(name);
		}

		memset(lineRaw, 0, sizeof(line));
	}

	//Parse mtl libs
	error_e e = parseMaterialLibs(filename, mtllibs, materialTextureMap);
	if(e != ERR_NONE)
		return e;


	//write triangles and parent bones
	for(const ObjFace &face : faces)
	{
		for(int i = 0; i < face.cornerCount-2; i++)
		{
			int ids[3] = {0, i+1, i+2};
			rpm_triangle_t tri;
			for(int k = 0; k < 3; k++)
			{
				ObjCorner &corner = corners[face.cornerId + ids[k]];
				tri.vertex3[k].pos3 = verts[corner.v - 1];
				tri.vertex3[k].norm3 = norms[corner.n - 1];
				tri.vertex3[k].uv2 = uvs[corner.u - 1];
				m_parentBones.push_back(face.groupId);
			}
			tri.iTexID = face.materialId;
			calcTriangleNormal(tri);
			m_triangles.push_back(tri);
		}
	}

	//write bones
	for(int i = 0; i < (int)groups.size(); i++)
		m_bones.push_back( CBone(groups[i], i, -1) );

	//write textures
	for(const std::string &material : materials)
		m_textures.push_back(materialTextureMap[material]);

	return ERR_NONE;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
error_e CObjInputFile::parseMaterialLibs( const std::string &objFilename, const std::vector<std::string> &mtllibs,
										 std::map<std::string, std::string, LessThanCppStringObj> &materialTextureMap )
{
	//get path of .obj file
	std::string path;
	size_t lastSlash = objFilename.find_last_of("/\\");
	if(lastSlash != std::string::npos)
		path.assign(objFilename, 0, lastSlash+1);

	for(std::string mtlFilename : mtllibs)
	{
		//use only the filename
		size_t lastSlash = mtlFilename.find_last_of("/\\");
		if(lastSlash != string::npos)
			mtlFilename.erase(0, lastSlash+1);
		std::string filename = path + mtlFilename;
		std::ifstream is(filename);
		if(!is.is_open())
			return ERR_FILENOTFOUND;
		char lineRaw[256] = {0};
		char currentMaterial[128] = {0};
		while(!is.getline(lineRaw, sizeof(lineRaw)).fail())
		{
			char *line = lineRaw;
			while(*line == ' ' || *line == '\t') //ignore whitespaces
				line++;

			if(strncmp(line, "newmtl ", 7) == 0)
			{
				if(sscanf(line, "newmtl %s", currentMaterial) != 1)
					return ERR_CORRUPT_FILE;
			}
			else if(strncmp(line, "map_Kd ", 7) == 0)
			{
				char name[256];
				if(sscanf(line, "map_Kd %s", name) != 1)
					return ERR_CORRUPT_FILE;
				std::string strName(name);
				size_t lastSlash = strName.find_last_of("/\\");
				if(lastSlash != string::npos)
					strName.erase(0, lastSlash+1);
				materialTextureMap[currentMaterial] = strName;
			}

			memset(lineRaw, 0, sizeof(lineRaw));
		}
	}

	return ERR_NONE;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::vector<rpm_triangle_t> & CObjInputFile::triangles()
{
	return m_triangles;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::vector<int> CObjInputFile::parentBones()
{
	return m_parentBones;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::vector<std::string> & CObjInputFile::textures()
{
	return m_textures;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::vector<CBone> & CObjInputFile::bones()
{
	return m_bones;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CObjInputFile::clear()
{
	m_triangles.clear();
	m_parentBones.clear();
	m_textures.clear();
	m_bones.clear();
}





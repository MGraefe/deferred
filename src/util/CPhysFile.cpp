// util/CPhysFile.cpp
// util/CPhysFile.cpp
// util/CPhysFile.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "CPhysFile.h"
#include "CPhysConvexCompoundShape.h"
#include "CPhysConvexHull.h"
#include "instream.h"

const short int CPhysFile::version(1);
const UINT CPhysFile::max_shapes(128);
const UINT CPhysFile::file_ident(((UINT)'S'<<24)+((UINT)'Y'<<16)+((UINT)'H'<<8)+(UINT)'P');


CPhysFile::CPhysFile() :
	centerOfMass(vec3_null),
	mass(100.0f)
{

}


CPhysFile::~CPhysFile()
{
	for(CPhysConvexCompoundShape* shape : m_physShapes)
	{
		if(shape)
			delete shape;
		shape = NULL;
	}
}


error_e CPhysFile::Serialize(const char *filename)
{
	error_e err = ERR_NONE;
	std::ofstream os(filename, std::ios_base::binary);
	
	if( os.is_open() )
		err = Serialize(os);
	else
		err = ERR_FILENOTFOUND;
	os.close();

	if( err != ERR_NONE )
		error("Error: Saving File \"%s\" failed: %s", filename, ToString(err));
	return err;
}



error_e CPhysFile::Serialize(std::ostream &os)
{
	WRITE_BINARY(os, file_ident);
	WRITE_BINARY(os, version);
	WRITE_BINARY(os, centerOfMass);
	WRITE_BINARY(os, mass);
	UINT size = (UINT)m_physShapes.size();
	WRITE_BINARY(os, size);
	for( const CPhysConvexCompoundShape *shape : m_physShapes )
	{
		error_e ret = shape->Serialize(os);
		if( ret != ERR_NONE )
			return ret;
	}

	return ERR_NONE;
}


error_e CPhysFile::Deserialize(const char *filename)
{
	error_e err = ERR_NONE;
	fs::ifstream is(filename, std::ios_base::binary);
	
	if( is.is_open() )
		err = Deserialize(is);
	else
		err = ERR_FILENOTFOUND;

	if( err != ERR_NONE )
		error("Error: Loading File \"%s\" failed: %s", filename, ToString(err));
	return err;
}


error_e CPhysFile::Deserialize(std::istream &is)
{
	if( !is.good() )
		return ERR_CORRUPT_FILE;

	UINT ident = 0;
	READ_BINARY(is, ident);
	if( ident != file_ident )
		return ERR_CORRUPT_FILE;

	short int fileVersion;
	READ_BINARY(is, fileVersion);
	if( fileVersion != version )
		return ERR_WRONG_VERSION;
	if( !is.good() )
		return ERR_CORRUPT_FILE;

	READ_BINARY(is, centerOfMass);
	READ_BINARY(is, mass);

	UINT numShapes;
	READ_BINARY(is, numShapes);

	if( !is.good() || numShapes > max_shapes )
		return ERR_CORRUPT_FILE;

	for(UINT i = 0; i < numShapes; i++ )
	{
		CPhysConvexCompoundShape *shape = new CPhysConvexCompoundShape();
		error_e ret = shape->Deserialize(is);
		if( ret != ERR_NONE )
		{
			delete shape;
			return ret;
		}
		m_physShapes.push_back(shape);
	}

	return ERR_NONE;
}
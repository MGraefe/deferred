// util/rpm_file.cpp
// util/rpm_file.cpp
// util/rpm_file.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "basic_types.h"
#include "rpm_file.h"
#include "instream.h"


//---------------------------------------------------------------
// Purpose: Returns zero on success
//---------------------------------------------------------------
rpm_file_t::rpm_load_errors_e  rpm_file_t::LoadFromFile( const char *filename )
{
	fs::ifstream is;
	is.open( filename, std::ios::binary );
	if( !is.is_open() )
		return RPM_LOAD_FILENOTFOUND;

	rpm_load_errors_e er = Deserialize(is);
	is.close();
	return er;
}


////---------------------------------------------------------------
//// Purpose: Returns zero on success
////---------------------------------------------------------------
//rpm_file_t::rpm_load_errors_e rpm_file_t::LoadFromFile( const wchar_t *filename )
//{
//	fs::ifstream is;
//	is.open( CAST_WCHAR_CONST(filename), std::ios::binary );
//	if( !is.is_open() )
//		return RPM_LOAD_FILENOTFOUND;
//
//	rpm_load_errors_e er = Deserialize(is);
//	is.close();
//	return er;
//}



//---------------------------------------------------------------
// Purpose: Returns zero on success
//---------------------------------------------------------------
#define CHECK_READ_GOOD() if( !is.good() ) { clearHeap(); return RPM_LOAD_CORRUPTFILE; }

rpm_file_t::rpm_load_errors_e rpm_file_t::Deserialize( std::istream &is )
{
	is.read( (char*)&header, sizeof(rpm_header_t) );
	CHECK_READ_GOOD();

	if( getVersion() != 4 )
		return RPM_LOAD_WRONGVERSION;

	if(!lumps.deserialize(is))
		return RPM_LOAD_CORRUPTFILE;

	rpm_lump *triangleLump = findLump(LUMP_TRIANGLES);
	if(triangleLump)
		triangles = (rpm_triangle_t*)triangleLump->getData();

	rpm_lump *texofsLump = findLump(LUMP_TEXOFS);
	if(texofsLump)
		texofs = (int*)texofsLump->getData();

	rpm_lump *texnamesLump = findLump(LUMP_TEXNAMES);
	if(texnamesLump)
		texnames = texnamesLump->getData();

	rpm_lump *skyVisLump = findLump(LUMP_SKYVIS);
	if(skyVisLump)
		skyVisibilities = (float*)skyVisLump->getData();

	return RPM_LOAD_OK;
}


//---------------------------------------------------------------
// Purpose: Returns zero on success
//---------------------------------------------------------------
#define CHECK_WRITE_GOOD() if( !os.good() ) { return RPM_WRITE_ERROR; }
rpm_file_t::rpm_write_errors_e rpm_file_t::Serialize( std::ostream &os )
{
	header.version = 4;
	os.write( (char*)&header, sizeof(rpm_header_t) );							CHECK_WRITE_GOOD();
	
	if(!lumps.serialize(os))
	{
		return RPM_WRITE_ERROR;
	}

	return RPM_WRITE_OK;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
rpm_lump * rpm_file_t::findLump( rpm_lumps_e lump ) const
{
	return lumps.findLump(lump);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const AABoundingBox & rpm_file_t::getBounds( void ) const
{
	return header.bounds;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float rpm_file_t::getSkyVisibility( int triangle, int vertex ) const
{
	Assert(triangle >= 0 && triangle < getTriangleCount());
	Assert(vertex >= 0 && vertex < 3);
	if( !skyVisibilities )
		return 1.0f;
	return skyVisibilities[triangle*3+vertex];
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int rpm_file_t::getVersion( void ) const
{
	return header.version;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int rpm_file_t::getTexDataLength( void ) const
{
	return header.iTexdataLength;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int rpm_file_t::getTextureCount( void ) const
{
	return header.sTexOfsetts;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int rpm_file_t::getTriangleCount( void ) const
{
	return header.num_of_triangles;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const char * rpm_file_t::getTexName( const rpm_triangle_t &triangle ) const
{
	return getTexName( triangle.iTexID );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const char * rpm_file_t::getTexName( int texID ) const
{
	if( !texnames || !texofs )
		return NULL;
	Assert(texID >= 0 && texID < getTextureCount());
	return &texnames[texofs[texID]];
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
rpm_file_t::~rpm_file_t()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
rpm_file_t::rpm_file_t()
{
	triangles = NULL;
	texofs = NULL;
	texnames = NULL;
	skyVisibilities = NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
rpm_file_t::rpm_file_t( const rpm_file_t &other ) :
	header(other.header)
{
	triangles = NULL;
	texofs = NULL;
	texnames = NULL;
	skyVisibilities = NULL;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
rpm_file_t &rpm_file_t::operator=(const rpm_file_t &other)
{
	header = other.header;
	triangles = other.triangles;
	texofs = other.texofs;
	texnames = other.texnames;
	skyVisibilities = other.skyVisibilities;
	return *this;
}


//---------------------------------------------------------------
// Purpose: Creates a lump, delete existing one if any
//---------------------------------------------------------------
rpm_lump * rpm_file_t::createLump( rpm_lumps_e ident )
{
	//invalidate convenience pointers
	switch(ident)
	{
	case LUMP_TRIANGLES:	triangles = NULL;		break;
	case LUMP_TEXOFS:		texofs = NULL;			break;
	case LUMP_TEXNAMES:		texnames = NULL;		break;
	case LUMP_SKYVIS:		skyVisibilities = NULL;	break;
	default: break;
	}
	return lumps.addLump(rpm_lump(ident));
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void rpm_file_t::clearHeap()
{
	lumps.clear();
	triangles = NULL;
	texofs = NULL;
	texnames = NULL;
	skyVisibilities = NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
rpm_lump * rpm_file_t::addLump( const rpm_lump &lump )
{
	return lumps.addLump(lump);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
rpm_lumps::rpm_lumps()
{

}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
rpm_lumps::~rpm_lumps()
{

}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool rpm_lumps::serialize( std::ostream &os )
{
	UINT size = lumps.size();
	os.write((char*)&size, sizeof(size));	if(!os.good()) return false;
	UINT dataStart = UINT(os.tellp()) + size * rpm_lump::headerSize();
	for(rpm_lump *l : lumps)
		if(!l->serializeHeader(os, dataStart))
			return false;
	for(rpm_lump *l : lumps)
		if(!l->serializeData(os))
			return false;
	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool rpm_lumps::deserialize( std::istream &is )
{
	for(rpm_lump *l : lumps)
		delete l;
	lumps.clear();
	UINT size = 0;
	is.read((char*)&size, sizeof(size));  if(!is.good()) return false;
	lumps.resize(size);
	for(UINT i = 0; i < size; i++)
	{
		lumps[i] = new rpm_lump();
		if(!lumps[i]->deserialize(is))
			return false;
	}
	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
rpm_lump * rpm_lumps::findLump( rpm_lumps_e lump ) const
{
	for(rpm_lump *l : lumps)
		if(l->getIdent() == lump)
			return l;
	return NULL;
}


//---------------------------------------------------------------
// Purpose: Add a lump, delete existing one if present
//---------------------------------------------------------------
rpm_lump *rpm_lumps::addLump( const rpm_lump &lump )
{
	auto it = std::find_if(lumps.begin(), lumps.end(), [&](const rpm_lump *l){return l->getIdent() == lump.getIdent();});
	if(it != lumps.end())
	{
		delete *it;
		lumps.erase(it);
	}
	lumps.push_back(new rpm_lump(lump));
	return lumps.back();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void rpm_lumps::clear()
{
	for(rpm_lump *lump : lumps)
		delete lump;
	lumps.clear();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
rpm_lump::rpm_lump() :
	m_ident(-1),
	m_size(-1),
	m_data(NULL, std::default_delete<char[]>()),
	m_start(-1)
{

}


//---------------------------------------------------------------
// Purpose: data array is now memory-managed by this class
//---------------------------------------------------------------
rpm_lump::rpm_lump( UINT ident, UINT size, char *data ) :
	m_ident(ident),
	m_size(size),
	m_data(data, std::default_delete<char[]>()),
	m_start(-1)
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
rpm_lump::rpm_lump( UINT ident ) :
	m_ident(ident),
	m_size(-1),
	m_data(NULL, std::default_delete<char[]>()),
	m_start(-1)
{

}


////---------------------------------------------------------------
//// Purpose: 
////---------------------------------------------------------------
//rpm_lump::rpm_lump( const rpm_lump &other ) :
//	ident(other.ident),
//	size(other.size),
//	data(other.data),
//	start(other.start)
//{
//
//}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
rpm_lump::~rpm_lump()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool rpm_lump::serializeHeader(std::ostream &os, UINT &dataStart)
{
	Assert(m_size != -1);
	m_start = dataStart;
	os.write((char*)&m_ident, sizeof(m_ident));	if(!os.good()) return false;
	os.write((char*)&m_size, sizeof(m_size));	if(!os.good()) return false;
	os.write((char*)&m_start, sizeof(m_start));	if(!os.good()) return false;
	dataStart += m_size;
	return os.good();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool rpm_lump::serializeData(std::ostream &os)
{
	Assert(m_size != -1);
	Assert((UINT)os.tellp() == m_start);
	os.write(m_data.get(), m_size);
	return os.good();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool rpm_lump::deserialize( std::istream &is )
{
	is.read((char*)&m_ident, sizeof(m_ident));	if(!is.good()) return false;
	if(m_ident > LUMP_COUNT__)
		return false;
	is.read((char*)&m_size, sizeof(m_size));	if(!is.good()) return false;
	is.read((char*)&m_start, sizeof(m_start));	if(!is.good()) return false;
	alloc(m_size);
	std::streamoff pos = is.tellg();
	is.seekg(m_start, is.beg);					if(!is.good()) return false;
	is.read(m_data.get(), m_size);				if(!is.good()) return false;
	is.seekg(pos, is.beg);
	return is.good();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void rpm_lump::alloc( UINT size )
{
	m_size = size;
	m_data.reset(new char[size], std::default_delete<char[]>());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
char * rpm_lump::getData() const
{
	return m_data.get();
}


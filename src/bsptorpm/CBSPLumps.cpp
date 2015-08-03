// bsptorpm/CBSPLumps.cpp
// bsptorpm/CBSPLumps.cpp
// bsptorpm/CBSPLumps.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "CBSPLumps.h"

using std::cout;
using std::endl;

//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
void CBSPLumps::coutImportantLumpSizes() 
{
	cout << "\nMap Counts:" << endl;
	cout << "Brushes: " << brushes.elements << endl;
	cout << "Brushsides: " << brushsides.elements << endl;
	cout << "Planes: " << planes.elements << endl;
	cout << "Faces: " << faces.elements << endl;
	cout << "Original Faces: " << origfaces.elements << endl;
	cout << "Edges: " << edges.elements << endl;
	cout << "Vertexes: " << vertexes.elements << endl;
	cout << "Textures: " << texinfos.elements << endl;
	cout << "Displacements: " << dispinfos.elements << endl;
	cout << "Displacement Vertexes: " << dispverts.elements << endl;
	cout << "Unique Textures: " << texStringTable.elements << endl;
}


//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
bool CBSPLumps::read(std::istream &is)
{
	is.seekg(0, std::ios::beg);
	is.read((char*)&header, sizeof(header));

	if(is.eof() || header.ident != IDBSPHEADER)
	{
		cout<<"Unknown File-Format." << endl;
		return false;
	}

	char ident[5] = {0};
	strncpy( ident, (char*)&header.ident, 4 );
	cout << "Ident: " << ident << endl;
	cout << "Version: " << header.version << endl;
	cout << "Map Revision:" << header.mapRevision << endl;

	if( header.version != 20 && header.version != 19 )
	{
		cout << "Unsupported Version, only BSP Versions 19 and 20 are currently supported." << endl;
		return false;
	}

	try
	{
		vertexes.read(		LUMP_VERTEXES, is, header);
		edges.read(			LUMP_EDGES, is, header);
		faces.read(			LUMP_FACES, is, header);
		origfaces.read(		LUMP_ORIGINALFACES, is, header);
		surfedges.read(		LUMP_SURFEDGES, is, header);
		texinfos.read(		LUMP_TEXINFO, is, header);
		texdatas.read(		LUMP_TEXDATA, is, header);
		dispinfos.read(		LUMP_DISPINFO, is, header);
		dispverts.read(		LUMP_DISP_VERTS, is, header);
		texStringTable.read(LUMP_TEXDATA_STRING_TABLE, is, header);
		texStringData.read(	LUMP_TEXDATA_STRING_DATA, is, header);
		entData.read(		LUMP_ENTITIES, is, header);
		brushes.read(		LUMP_BRUSHES, is, header);
		brushsides.read(	LUMP_BRUSHSIDES, is, header);
		planes.read(		LUMP_PLANES, is, header);
	} 
	catch( std::exception &e )
	{
		cout << "ERROR: " << e.what() << endl;
		return false;
	}

	coutImportantLumpSizes();

	return true;
}

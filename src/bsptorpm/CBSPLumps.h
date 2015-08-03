// bsptorpm/CBSPLumps.h
// bsptorpm/CBSPLumps.h
// bsptorpm/CBSPLumps.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__bsptorpm__CBSPLumps_H__
#define deferred__bsptorpm__CBSPLumps_H__

#include "bspfile.h"
#include <iostream>

template<typename T>
class CBSPLump
{
public:
	CBSPLump() {
		elementsize = sizeof(T);
		name = datasize = elements = -1;
		addr = NULL;
	}

	T *get( void ) { 
		return addr; 
	}

	template<typename U>
	T &get( U i ) {
		Assert( i >= 0 && i < elements );
		return addr[i];
	}

	template<typename U>
	T &operator[]( U i ) {
		return get(i);
	}

	template<typename U>
	const T &get( U i ) const {
		Assert( i >= 0 && i < elements );
		return addr[i];
	}

	template<typename U>
	const T &operator[]( U i ) const {
		return get(i);
	}

	void read(int name, std::istream &is, dheader_t &header) {
		this->name = name;
		datasize = header.lumps[name].filelen;
		elements = datasize / elementsize;
		addr = new T[elements];

		is.seekg( header.lumps[name].fileofs, std::ios::beg );
		if( is.good() )
			is.read( (char*)addr, datasize );
		else
			throw std::exception("Corrupt File");
	}

	operator T*() {
		return get();
	}

public:
	int name;
	int datasize;
	int elementsize;
	int elements;
	T *addr;
};


class CBSPLumps
{
public:
	CBSPLumps() {
		memset((void*)&header, 0, sizeof(header));
	}

	bool read(std::istream &is);

private:
	void coutImportantLumpSizes();

public:
	dheader_t header;
	CBSPLump<dvertex_t> vertexes;
	CBSPLump<dedge_t> edges;
	CBSPLump<dface_t> faces;
	CBSPLump<dface_t> origfaces;
	CBSPLump<int> surfedges;
	CBSPLump<texinfo_t> texinfos;
	CBSPLump<dtexdata_t> texdatas;
	CBSPLump<ddispinfo_t> dispinfos;
	CBSPLump<CDispVert> dispverts;
	CBSPLump<int> texStringTable;
	CBSPLump<char> texStringData;
	CBSPLump<char> entData;
	CBSPLump<dbrush_t> brushes;
	CBSPLump<dbrushside_t> brushsides;
	CBSPLump<dplane_t> planes;
};

#endif

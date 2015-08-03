// renderer/TGA.cpp
// renderer/TGA.cpp
// renderer/TGA.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "TGA.h"
#include "glheaders.h"
#include <util/instream.h>
#include <util/error.h>
#include "CRenderInterf.h"
#include "TextureManager.h"
#include "CLoadingMutex.h"

void ReadCompressed( BYTE *pTexture, fs::ifstream *file, int iPixelCount, int iBytesPerPixel );


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void Fehler( const char* message )
{
	error_fatal( message );
};


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
int g_iFiltering;
void SetTextureFiltering( int filter )
{
	g_iFiltering = filter;
}


//-----------------------------------------------------------------------
// Purpose: Loads a BGR/BGRA Texture into Video-Mem and sets filtering Method
//-----------------------------------------------------------------------
void BindTexture( int index, BYTE *pTexture, int width, int height, int BytesPerPixel, int textureTarget, bool srgb )
{
	g_LoadingMutex->SetOrWait();

	int texTarget1 = textureTarget;
	int texTarget2 = textureTarget;
	if( textureTarget >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && textureTarget <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z )
		texTarget2 = GL_TEXTURE_CUBE_MAP;

	glBindTexture( texTarget2, index );

	if( g_iFiltering == GL_LINEAR_MIPMAP_LINEAR )
		glTexParameteri( texTarget2, GL_GENERATE_MIPMAP, GL_TRUE );

	//Filtering
	glTexParameteri(texTarget2, GL_TEXTURE_MAG_FILTER, g_iFiltering);
	glTexParameteri(texTarget2, GL_TEXTURE_MIN_FILTER, g_iFiltering);

	int format;
	int internalformat;

	if( BytesPerPixel == 4 )
	{
		format = GL_BGRA_EXT;
		//internalformat = GL_COMPRESSED_RGBA_ARB;
		internalformat = srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
	}
	else if( BytesPerPixel == 3 )
	{
		format = GL_BGR_EXT;
		//internalformat = GL_COMPRESSED_RGB_ARB;
		internalformat = srgb ? GL_SRGB8 : GL_RGB8;
	}
	else
	{
		error( "Unknown Format in Texture" );
		return;
	}

	glTexImage2D(texTarget1, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, pTexture);
	
	g_LoadingMutex->Release();
}


//-----------------------------------------------------------------------
// Purpose: Loads a TGA File from Disk.
//			Supports: BGR TGA24, BGRA TGA32, BGR TGA24 (RLE compressed),
//			BGRA TGA32 (RLE compressed )
//-----------------------------------------------------------------------
bool PrecacheTgaImage( const char *pFilename, int index, texture_t &tex, int textureTarget, bool flipY, bool srgb )
{
	tex.filename = pFilename;
	tex.index = index;

	//Load a TGA-Image from Disk
	int i;

	tgafile_t tgaImage;

	//Open File!
	fs::ifstream ffsfile( tex.filename, std::ios::binary );

	if( ffsfile.bad() || !ffsfile.is_open() )
	{
		ffsfile.close();
		//Fehler( "File is invalid" );
		return false;
	}

	//Read out the Targa-File
	//For docs see tga.h

	//-----Header
	ffsfile.read((char*)&tgaImage.CharsInIdentField, sizeof(tgaImage.CharsInIdentField));
	ffsfile.read((char*)&tgaImage.ColorMapType, sizeof(tgaImage.ColorMapType));
	ffsfile.read((char*)&tgaImage.ImageType, sizeof(tgaImage.ImageType));
	if( ( tgaImage.ColorMapType != 0 ) && ( tgaImage.ColorMapType != 1 ) )
	{
		ffsfile.close();
		Fehler( "Invalid Image" );
		return false;
    }
	if( tgaImage.ImageType != 2 && tgaImage.ImageType != 10 ) //2 = uncompressed RGB, 10 = RLE RGB
	{
		ffsfile.close();
		Fehler( "Wrong type of Image, only type 2(unmapped RGB image) and 10(unmapped compressed RGB image) is supported!" );
		return false;
	}

	//-----color map specifications
	ffsfile.read((char*)&tgaImage.colormapspec, 5 ); //5 Byte Size

	//-----Image specifications
	ffsfile.read((char*)&tgaImage.imagespec, 10 ); //10 Byte Size

	if( tgaImage.imagespec.pixelSize < 24 || tgaImage.imagespec.pixelSize > 32 )
	{
		ffsfile.close();
		Fehler( "Wrong type of Image, only 24 and 32 bits per pixel are supported!" );
		return false;
	}
	tgaImage._hasalpha = (tgaImage.imagespec.pixelSize == 32) ? true : false;

	//-----Identification String
	tgaImage.imageident.pIdentString = NULL;
	if( tgaImage.CharsInIdentField > 0 )
	{
		//Setze die Größe des Ident-Strings
		tgaImage.imageident.pIdentString = new char[tgaImage.CharsInIdentField];
		//Lese Ident-String!
		ffsfile.read((char*)tgaImage.imageident.pIdentString, tgaImage.CharsInIdentField);
	}

	//-----Color-Map
	if( tgaImage.ColorMapType != 0 )
	{
		tgaImage.colormap.pB = new BYTE[tgaImage.colormapspec.length];
		tgaImage.colormap.pG = new BYTE[tgaImage.colormapspec.length];
		tgaImage.colormap.pR = new BYTE[tgaImage.colormapspec.length];
		tgaImage.colormap.pA = new BYTE[tgaImage.colormapspec.length];

		for(i = 0; i < tgaImage.colormapspec.length; i++)
		{
			ffsfile.read((char*)&tgaImage.colormap.pB[i], sizeof(tgaImage.colormap.pB[i]));
			ffsfile.read((char*)&tgaImage.colormap.pG[i], sizeof(tgaImage.colormap.pG[i]));
			ffsfile.read((char*)&tgaImage.colormap.pR[i], sizeof(tgaImage.colormap.pR[i]));
			if( tgaImage.colormapspec.size > 24 )
				ffsfile.read((char*)&tgaImage.colormap.pA[i], sizeof(tgaImage.colormap.pA[i]));
		}
	}

	BYTE *pTexture;
	pTexture = NULL;

	//-----Image Data
	int iPixelCount = tgaImage.imagespec.width * tgaImage.imagespec.heigth;
	if( tgaImage.ImageType == 2 )
	{
		if( tgaImage._hasalpha ) //Targa 32
		{
			pTexture = new BYTE[iPixelCount * 4];
			ffsfile.read((char*)pTexture, iPixelCount * 4 ); //4 Bytes Size
			BindTexture( tex.index, pTexture, tgaImage.imagespec.width, tgaImage.imagespec.heigth, 4, textureTarget, srgb );
		}
		else //Targa 24
		{
			pTexture = new BYTE[iPixelCount * 3];
			ffsfile.read((char*)pTexture, iPixelCount * 3 ); //4 Bytes Size
			BindTexture( tex.index, pTexture, tgaImage.imagespec.width, tgaImage.imagespec.heigth, 3, textureTarget, srgb );
		}
	}
	else //Compressed BGR/BGRA
	{
		if( tgaImage._hasalpha ) //Targa 32
		{
			pTexture = new BYTE[iPixelCount*4];
			ReadCompressed( pTexture, &ffsfile, iPixelCount, 4 );
			BindTexture( tex.index, pTexture, tgaImage.imagespec.width, tgaImage.imagespec.heigth, 4, textureTarget, srgb );
		}
		else //Targa 24
		{
			pTexture = new BYTE[iPixelCount*3];
			ReadCompressed( pTexture, &ffsfile, iPixelCount, 3 );
			BindTexture( tex.index, pTexture, tgaImage.imagespec.width, tgaImage.imagespec.heigth, 3, textureTarget, srgb );
		}
	}
    
	ffsfile.close();

	tex.height = abs(tgaImage.imagespec.heigth);
	tex.width = abs(tgaImage.imagespec.width);

	delete[] pTexture;

	if( tgaImage.CharsInIdentField > 0 )
		delete[] tgaImage.imageident.pIdentString;

	if( tgaImage.ColorMapType != 0 )
	{
		delete [] tgaImage.colormap.pB;
		delete [] tgaImage.colormap.pG;
		delete [] tgaImage.colormap.pR;
		delete [] tgaImage.colormap.pA;
	}
	return true;
}


//Header for the TGA RLE Control-Byte
//First Bit indicates if Compressed or not, 7 following indicate the count of data
class CRLEHeader
{
public:
	void	Set( const BYTE val );
	bool	Cpr( void ) const;
	int		Num( void ) const;
	BYTE	*Get( void );
	BYTE m_val;
};

void CRLEHeader::Set( const BYTE val )
{
	m_val = val;
}

bool CRLEHeader::Cpr( void ) const
{
	return (m_val & 128) != 0;
}

int CRLEHeader::Num( void ) const
{
	if( Cpr() )
		return m_val - 128 + 1;
	else
		return m_val + 1;
}

BYTE *CRLEHeader::Get( void )
{
	return &m_val;
}


//-----------------------------------------------------------------------
// Purpose: Loads a Run-Length-Compressed TGA
//			*file must point to a ifstream class. The File Pointer of
//			this Class must be at the beginning of the Image Data of
//			the TGA file.
//-----------------------------------------------------------------------
void ReadCompressed( BYTE *pTexture, fs::ifstream *file, int iPixelCount, int iBytesPerPixel )
{
	BYTE *pTex = pTexture;
	int iPixels = 0;
	char *pColor = new char[iBytesPerPixel];
	CRLEHeader rle;

	while( iPixels < iPixelCount )
	{
		file->read( (char*)&rle.m_val, 1 ); //Read Packet Header(1 Byte)
		if( rle.Cpr() ) //Packet is a compressed Packet, indicating num() pixels of the same color
		{
			file->read( pColor, iBytesPerPixel );
			for( int i = 0; i < rle.Num(); i++ )
			{
				memcpy( pTex, pColor, iBytesPerPixel );
				pTex += iBytesPerPixel;
			}
			iPixels += rle.Num();
		}
		else //Packet is a raw Packet, indicating num() pixels of different colors following
		{
			file->read( (char*)pTex, rle.Num() * iBytesPerPixel );
			pTex += rle.Num() * iBytesPerPixel;
			iPixels += rle.Num();
		}
	}
    delete[] pColor;	
}

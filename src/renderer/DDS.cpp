// renderer/DDS.cpp
// renderer/DDS.cpp
// renderer/DDS.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "DDS.h"
#include <util/instream.h>
#include <util/error.h>
#include "glheaders.h"
#include "CLoadingMutex.h"

#ifdef _WINDOWS
//#define INCLUDE_DDRAW
#endif

#ifdef INCLUDE_DDRAW
#include <ddraw.h>
#else

#define DDSD_MIPMAPCOUNT 0x00020000l

#ifdef MAKEFOURCC
#undef MAKEFOURCC
#endif
#define MAKEFOURCC(x0, x1, x2, x3) \
	((UINT)(BYTE)(x0) | (UINT)(BYTE)(x1) << 8 | (UINT)(BYTE)(x2) << 16 | (UINT)(BYTE)(x3) << 24)

#define FOURCC_DXT1  (MAKEFOURCC('D','X','T','1'))
#define FOURCC_DXT2  (MAKEFOURCC('D','X','T','2'))
#define FOURCC_DXT3  (MAKEFOURCC('D','X','T','3'))
#define FOURCC_DXT4  (MAKEFOURCC('D','X','T','4'))
#define FOURCC_DXT5  (MAKEFOURCC('D','X','T','5'))

struct DDCOLORKEY
{
	UINT       dwColorSpaceLowValue;
	UINT       dwColorSpaceHighValue;
};

struct DDSCAPS2
{
	UINT       dwCaps;         // capabilities of surface wanted
	UINT       dwCaps2;
	UINT       dwCaps3;
	union
	{
		UINT       dwCaps4;
		UINT       dwVolumeDepth;
	};
};

struct DDPIXELFORMAT
{
	UINT       dwSize;                 // size of structure
	UINT       dwFlags;                // pixel format flags
	UINT       dwFourCC;               // (FOURCC code)
	union
	{
		UINT   dwRGBBitCount;          // how many bits per pixel
		UINT   dwYUVBitCount;          // how many bits per pixel
		UINT   dwZBufferBitDepth;      // how many total bits/pixel in z buffer (including any stencil bits)
		UINT   dwAlphaBitDepth;        // how many bits for alpha channels
		UINT   dwLuminanceBitCount;    // how many bits per pixel
		UINT   dwBumpBitCount;         // how many bits per "buxel", total
		UINT   dwPrivateFormatBitCount;// Bits per pixel of private driver formats. Only valid in texture
		// format list and if DDPF_D3DFORMAT is set
	};
	union
	{
		UINT   dwRBitMask;             // mask for red bit
		UINT   dwYBitMask;             // mask for Y bits
		UINT   dwStencilBitDepth;      // how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
		UINT   dwLuminanceBitMask;     // mask for luminance bits
		UINT   dwBumpDuBitMask;        // mask for bump map U delta bits
		UINT   dwOperations;           // DDPF_D3DFORMAT Operations
	};
	union
	{
		UINT   dwGBitMask;             // mask for green bits
		UINT   dwUBitMask;             // mask for U bits
		UINT   dwZBitMask;             // mask for Z bits
		UINT   dwBumpDvBitMask;        // mask for bump map V delta bits
		struct
		{
			USHORT    wFlipMSTypes;       // Multisample methods supported via flip for this D3DFORMAT
			USHORT    wBltMSTypes;        // Multisample methods supported via blt for this D3DFORMAT
		} MultiSampleCaps;

	};
	union
	{
		UINT   dwBBitMask;             // mask for blue bits
		UINT   dwVBitMask;             // mask for V bits
		UINT   dwStencilBitMask;       // mask for stencil bits
		UINT   dwBumpLuminanceBitMask; // mask for luminance in bump map
	};
	union
	{
		UINT   dwRGBAlphaBitMask;      // mask for alpha channel
		UINT   dwYUVAlphaBitMask;      // mask for alpha channel
		UINT   dwLuminanceAlphaBitMask;// mask for alpha channel
		UINT   dwRGBZBitMask;          // mask for Z channel
		UINT   dwYUVZBitMask;          // mask for Z channel
	};
};

struct DDSURFACEDESC2
{
	UINT               dwSize;                 // size of the DDSURFACEDESC structure
	UINT               dwFlags;                // determines what fields are valid
	UINT               dwHeight;               // height of surface to be created
	UINT               dwWidth;                // width of input surface
	union
	{
		long            lPitch;                 // distance to start of next line (return value only)
		UINT           dwLinearSize;           // Formless late-allocated optimized surface size
	};
	union
	{
		UINT           dwBackBufferCount;      // number of back buffers requested
		UINT           dwDepth;                // the depth if this is a volume texture
	};
	union
	{
		UINT           dwMipMapCount;          // number of mip-map levels requestde
											   // dwZBufferBitDepth removed, use ddpfPixelFormat one instead
		UINT           dwRefreshRate;          // refresh rate (used when display mode is described)
		UINT           dwSrcVBHandle;          // The source used in VB::Optimize
	};
	UINT               dwAlphaBitDepth;        // depth of alpha buffer requested
	UINT               dwReserved;             // reserved
	void*              lpSurface;              // pointer to the associated surface memory
	union
	{
		DDCOLORKEY      ddckCKDestOverlay;      // color key for destination overlay use
		UINT           dwEmptyFaceColor;       // Physical color for empty cubemap faces
	};
	DDCOLORKEY          ddckCKDestBlt;          // color key for destination blt use
	DDCOLORKEY          ddckCKSrcOverlay;       // color key for source overlay use
	DDCOLORKEY          ddckCKSrcBlt;           // color key for source blt use
	union
	{
		DDPIXELFORMAT   ddpfPixelFormat;        // pixel format description of the surface
		UINT           dwFVF;                  // vertex format description of vertex buffers
	};
	DDSCAPS2            ddsCaps;                // direct draw surface capabilities
	UINT               dwTextureStage;         // stage in multitexture cascade
};


typedef enum _D3DFORMAT {
	D3DFMT_UNKNOWN              =  0,

	D3DFMT_R8G8B8               = 20,
	D3DFMT_A8R8G8B8             = 21,
	D3DFMT_X8R8G8B8             = 22,
	D3DFMT_R5G6B5               = 23,
	D3DFMT_X1R5G5B5             = 24,
	D3DFMT_A1R5G5B5             = 25,
	D3DFMT_A4R4G4B4             = 26,
	D3DFMT_R3G3B2               = 27,
	D3DFMT_A8                   = 28,
	D3DFMT_A8R3G3B2             = 29,
	D3DFMT_X4R4G4B4             = 30,
	D3DFMT_A2B10G10R10          = 31,
	D3DFMT_A8B8G8R8             = 32,
	D3DFMT_X8B8G8R8             = 33,
	D3DFMT_G16R16               = 34,
	D3DFMT_A2R10G10B10          = 35,
	D3DFMT_A16B16G16R16         = 36,

	D3DFMT_A8P8                 = 40,
	D3DFMT_P8                   = 41,

	D3DFMT_L8                   = 50,
	D3DFMT_A8L8                 = 51,
	D3DFMT_A4L4                 = 52,

	D3DFMT_V8U8                 = 60,
	D3DFMT_L6V5U5               = 61,
	D3DFMT_X8L8V8U8             = 62,
	D3DFMT_Q8W8V8U8             = 63,
	D3DFMT_V16U16               = 64,
	D3DFMT_A2W10V10U10          = 67,

	D3DFMT_UYVY                 = MAKEFOURCC('U', 'Y', 'V', 'Y'),
	D3DFMT_R8G8_B8G8            = MAKEFOURCC('R', 'G', 'B', 'G'),
	D3DFMT_YUY2                 = MAKEFOURCC('Y', 'U', 'Y', '2'),
	D3DFMT_G8R8_G8B8            = MAKEFOURCC('G', 'R', 'G', 'B'),
	D3DFMT_DXT1                 = MAKEFOURCC('D', 'X', 'T', '1'),
	D3DFMT_DXT2                 = MAKEFOURCC('D', 'X', 'T', '2'),
	D3DFMT_DXT3                 = MAKEFOURCC('D', 'X', 'T', '3'),
	D3DFMT_DXT4                 = MAKEFOURCC('D', 'X', 'T', '4'),
	D3DFMT_DXT5                 = MAKEFOURCC('D', 'X', 'T', '5'),

	D3DFMT_D16_LOCKABLE         = 70,
	D3DFMT_D32                  = 71,
	D3DFMT_D15S1                = 73,
	D3DFMT_D24S8                = 75,
	D3DFMT_D24X8                = 77,
	D3DFMT_D24X4S4              = 79,
	D3DFMT_D16                  = 80,

	D3DFMT_D32F_LOCKABLE        = 82,
	D3DFMT_D24FS8               = 83,

#if !defined(D3D_DISABLE_9EX)
	D3DFMT_D32_LOCKABLE         = 84,
	D3DFMT_S8_LOCKABLE          = 85,
#endif // !D3D_DISABLE_9EX

	D3DFMT_L16                  = 81,

	D3DFMT_VERTEXDATA           =100,
	D3DFMT_INDEX16              =101,
	D3DFMT_INDEX32              =102,

	D3DFMT_Q16W16V16U16         =110,

	D3DFMT_MULTI2_ARGB8         = MAKEFOURCC('M','E','T','1'),

	D3DFMT_R16F                 = 111,
	D3DFMT_G16R16F              = 112,
	D3DFMT_A16B16G16R16F        = 113,

	D3DFMT_R32F                 = 114,
	D3DFMT_G32R32F              = 115,
	D3DFMT_A32B32G32R32F        = 116,

	D3DFMT_CxV8U8               = 117,

#if !defined(D3D_DISABLE_9EX)
	D3DFMT_A1                   = 118,
	D3DFMT_A2B10G10R10_XR_BIAS  = 119,
	D3DFMT_BINARYBUFFER         = 199,
#endif // !D3D_DISABLE_9EX

	D3DFMT_FORCE_DWORD          =0x7fffffff
} D3DFORMAT;

#endif /* INCLUDE_DDRAW */

struct dds_image_data_t
{
	int  width;
	int  height;
	int  components;
	int  bitsPerPixel;
	UINT  encoding;
	GLenum  internalformat;
	int  numMipMaps;
	std::vector<unsigned char> pixels;
	bool bHasMipMaps;
	bool bIsCompressed;
	bool bIsFloatingPoint;
};

#define FLIP(a,b) tmp = block[a]; block[a] = block[b]; block[b] = tmp

//-----------------------------------------------------------------------
// A DXT1 block layout is:
// [0-1] color0.
// [2-3] color1.
// [4-7] color bitmap, 2 bits per pixel.
// So each of the 4-7 bytes represents one line, flipping a block is just
// flipping those bytes.
// Note that http://src.chromium.org/viewvc/chrome/trunk/src/o3d/core/cross/bitmap_dds.cc?view=markup&pathrev=21227
// contains an error in the last line: data[6]=data[5] is a bug!
//-----------------------------------------------------------------------
void FlipDXT1BlockFull(unsigned char *block)
{
	unsigned char tmp;
	FLIP(4,7);
	FLIP(5,6);
}

void FlipDXT1BlockHalf(unsigned char *block)
{
	unsigned char tmp;
	FLIP(4,5);
}


//-----------------------------------------------------------------------
// Purpose: Flips a full DXT3 block in the y direction.
// A DXT3 block layout is:
// [0-7]  alpha bitmap, 4 bits per pixel.
// [8-15] a DXT1 block.
//-----------------------------------------------------------------------
void FlipDXT3BlockFull(unsigned char *block)
{
	// We can flip the alpha bits at the byte level (2 bytes per line).
	unsigned char tmp;

	FLIP(0,6);
	FLIP(1,7);
	FLIP(2,4);
	FLIP(3,5);
	
	// And flip the DXT1 block using the above function.
	FlipDXT1BlockFull(block+8);
}

void FlipDXT3BlockHalf(unsigned char *block )
{
	unsigned char tmp;
	FLIP(0,2);
	FLIP(1,3);

	FlipDXT1BlockHalf(block+8);
}


//-----------------------------------------------------------------------
// A DXT5 block layout is:
// [0]    alpha0.
// [1]    alpha1.
// [2-7]  alpha bitmap, 3 bits per pixel.
// [8-15] a DXT1 block.

// The alpha bitmap doesn't easily map lines to bytes, so we have to
// interpret it correctly.  Extracted from
// http://www.opengl.org/registry/specs/EXT/texture_compression_s3tc.txt :
//
//   The 6 "bits" bytes of the block are decoded into one 48-bit integer:
//
//     bits = bits_0 + 256 * (bits_1 + 256 * (bits_2 + 256 * (bits_3 +
//                   256 * (bits_4 + 256 * bits_5))))
//
//   bits is a 48-bit unsigned integer, from which a three-bit control code
//   is extracted for a texel at location (x,y) in the block using:
//
//       code(x,y) = bits[3*(4*y+x)+1..3*(4*y+x)+0]
//
//   where bit 47 is the most significant and bit 0 is the least
//   significant bit.
//
// From http://src.chromium.org/viewvc/chrome/trunk/src/o3d/core/cross/bitmap_dds.cc?view=markup&pathrev=21227
// Original source contained bugs; fixed here.
//-----------------------------------------------------------------------
void FlipDXT5BlockFull(unsigned char *block)
{

	// From Chromium (source was buggy)
	unsigned int line_0_1 = block[2] + 256 * (block[3] + 256 * block[4]);
	unsigned int line_2_3 = block[5] + 256 * (block[6] + 256 * block[7]);
	// swap lines 0 and 1 in line_0_1.
	unsigned int line_1_0 = ((line_0_1 & 0x000fff) << 12) |
	((line_0_1 & 0xfff000) >> 12);
	// swap lines 2 and 3 in line_2_3.
	unsigned int line_3_2 = ((line_2_3 & 0x000fff) << 12) |
	((line_2_3 & 0xfff000) >> 12);
	block[2] = line_3_2 & 0xff;
	block[3] = (line_3_2 & 0xff00) >> 8;
	block[4] = (line_3_2 & 0xff0000) >> 16;
	block[5] = line_1_0 & 0xff;
	block[6] = (line_1_0 & 0xff00) >> 8;
	block[7] = (line_1_0 & 0xff0000) >> 16;


	// And flip the DXT1 block using the above function.
	FlipDXT1BlockFull(block+8);
}

void FlipDXT5BlockHalf(unsigned char *block)
{
	unsigned char b2 = block[2];
	unsigned char b3 = block[3];
	unsigned char b4 = block[4];

	block[2] = (b3 << 4) | (b4 >> 4);
	block[3] = (b4 << 4) | (b2 >> 4);
	block[4] = (b2 << 4) | (b3 >> 4);

	FlipDXT1BlockHalf(block+8);
}


int ImageByteSizeDXT( int width, int height, int blockSize )
{
	return ((width+3)/4) * ((height+3)/4) * blockSize;
}


// Flip & copy to actual pixel buffer
void FlipComprImageSingleLevel( int format, int blockSize, int width, int height, unsigned char *data )
{
	int w = width;
	int h = height;

	int totalData = ImageByteSizeDXT( width, height, blockSize );

	unsigned char *tmpData = new unsigned char[totalData];

	int i, widBytes, k;
	unsigned char *s, *d;

	widBytes = ((w+3)/4)*blockSize;
	s = data;
	d = tmpData + ((h+3)/4 - 1) * widBytes;

	if( h > 2 )
	{
		for( i = 0; i < (h+3)/4; i++ )
		{
			memcpy( d, s, widBytes );
		
			if( format == FOURCC_DXT1 )
			{
				for( k = 0; k < widBytes/blockSize; k++)
					FlipDXT1BlockFull( d + k*blockSize );
			} 
			else if( format == FOURCC_DXT3 )
			{
				for(  k = 0; k < widBytes/blockSize; k++ )
					FlipDXT3BlockFull( d + k*blockSize );
			}
			else if( format == FOURCC_DXT5 )
			{
				for( k = 0; k < widBytes/blockSize; k++ )
					FlipDXT5BlockFull( d + k*blockSize );
			}

			s += widBytes;
			d -= widBytes;
		}
	}
	else
	{
		for( i = 0; i < (h+3)/4; i++ )
		{
			memcpy( d, s, widBytes );

			if( format == FOURCC_DXT1 )
			{
				for( k = 0; k < widBytes/blockSize; k++)
					FlipDXT1BlockHalf( d + k*blockSize );
			} 
			else if( format == FOURCC_DXT3 )
			{
				for(  k = 0; k < widBytes/blockSize; k++ )
					FlipDXT3BlockHalf( d + k*blockSize );
			}
			else if( format == FOURCC_DXT5 )
			{
				for( k = 0; k < widBytes/blockSize; k++ )
					FlipDXT5BlockHalf( d + k*blockSize );
			}

			s += widBytes;
			d -= widBytes;
		}
	}

	//copy temporary data to data;
	memcpy( data, tmpData, totalData );

	delete[] tmpData;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void FlipUncompImageSingleLevel( unsigned char *data, UINT width, UINT height, UINT bitsPerPixel )
{
	Assert(height % 2 == 0);

	UINT lineSize = width * bitsPerPixel / 8;
	Assert(lineSize > 0);

	unsigned char *lineBuf = new unsigned char[lineSize];
	for(UINT i = 0; i < height/2; ++i)
	{
		unsigned char *linea = data + i * lineSize;
		unsigned char *lineb = data + (height - 1 - i) * lineSize;
		memcpy(lineBuf, linea, lineSize); // buf <= a
		memcpy(linea, lineb, lineSize);   // a <= b
		memcpy(lineb, lineBuf, lineSize); // b <= buf
	}
	delete[] lineBuf;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void FlipImage( dds_image_data_t &DDSImageData )
{
	int w = DDSImageData.width;
	int h = DDSImageData.height;

	int blockSize = 16; //Block size in bytes
	if( DDSImageData.encoding == FOURCC_DXT1 )
		blockSize = 8;

	unsigned char *data = DDSImageData.pixels.data();

	int iNumTexs = DDSImageData.bHasMipMaps ? DDSImageData.numMipMaps : 1;
	for( int i = 0; i < iNumTexs; i++ )
	{
		Assert( w > 0 || h > 0 );
		w = max(1, w);
		h = max(1, h);
		
		if(DDSImageData.bIsCompressed)
		{
			if( h > 1 )
				FlipComprImageSingleLevel( DDSImageData.encoding, blockSize, w, h, data ); //In place
			data += ImageByteSizeDXT( w, h, blockSize );
		}
		else
		{
			if( h > 1 )
				FlipUncompImageSingleLevel( data, w, h, DDSImageData.bitsPerPixel );
			data += w * h * DDSImageData.bitsPerPixel / 8;
		}
				
		w /= 2;
		h /= 2;
	}
}





//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void LoadIntoGpu( dds_image_data_t &DDSImageData, int textureTarget, int index )
{
	g_LoadingMutex->SetOrWait();

	glBindTexture( textureTarget, index );

	if( DDSImageData.numMipMaps == 0 )
	{
		glTexParameteri( textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( textureTarget, GL_GENERATE_MIPMAP, GL_FALSE );
	}
	else
	{
		glTexParameteri( textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( textureTarget, GL_TEXTURE_MAX_LEVEL, DDSImageData.numMipMaps - 1 );
	}

	glGetError();

	// Load the mip-map levels
	int iNumTexs = DDSImageData.bHasMipMaps ? DDSImageData.numMipMaps : 1;
	int nHeight = DDSImageData.height;
	int nWidth = DDSImageData.width;
	int nSize;
	int nOffset = 0;

	GLenum format;
	switch(DDSImageData.components)
	{
		case 1: format = GL_RED; break;
		case 2: format = GL_RG; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		default: format = 0;
	}

	GLenum type = DDSImageData.bIsFloatingPoint ? 
		(DDSImageData.bitsPerPixel/DDSImageData.components == 16 ? GL_HALF_FLOAT : GL_FLOAT) : 
		GL_UNSIGNED_BYTE;


	for( int i = 0; i < iNumTexs; i++ )
	{
		Assert( nHeight > 0 || nWidth > 0 );
		nWidth = max(1, nWidth);
		nHeight = max(1, nHeight);

		unsigned char *pixels = DDSImageData.pixels.data() + nOffset;

		if(DDSImageData.bIsCompressed)
		{
			nSize = ImageByteSizeDXT(nWidth, nHeight, DDSImageData.bitsPerPixel * 2);
			glCompressedTexImage2D( textureTarget, i, DDSImageData.internalformat, nWidth, nHeight,	0, nSize, pixels );
		}
		else
		{
			nSize = nWidth * nHeight * DDSImageData.bitsPerPixel / 8;
			glTexImage2D( textureTarget, i, DDSImageData.internalformat, nWidth, nHeight, 0, format, type, pixels);
		}

		GLenum err = glGetError();
		if(err != GL_NO_ERROR)
			error("opengl error while loading DDS texture to GPU");
		Assert( err == GL_NO_ERROR );

		nOffset += nSize;

		// Half the image size for the next mip-map level...
		nWidth  /= 2;
		nHeight /= 2;
	}
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool LoadDDSFromFile( const char *pFilename, int index, texture_t &tex, int textureTarget, bool flipY, bool srgb )
{
	dds_image_data_t DDSImageData;
	DDSURFACEDESC2 ddsd;
	char filecode[4];
	int mipmapFactor;
	int bufferSize;

	////////////////////////////////////
	//Load texture from file into ram
	////////////////////////////////////

	// Open the file
	fs::ifstream is( pFilename, std::ios::binary );

	if( !is.is_open() )
	{
		ConsoleMessage( "loadDDSTextureFile couldn't find, or failed to load \"%s\"", pFilename );
		is.close();
		return false;
	}

	is.seekg( 0, std::ios::beg );

	// Verify the file is a true .dds file
	is.read( filecode, 4 );
	//fread( filecode, 1, 4, pFile );

	if( strncmp( filecode, "DDS ", 4 ) != 0 )
	{
		error("The file \"%s\" doesn't appear to be a valid .dds file!", pFilename );
		is.close();
		return false;
	}

	// Get the surface descriptor
	is.read( (char*)&ddsd, sizeof(ddsd) );

	memset( &DDSImageData, 0, sizeof(dds_image_data_t) );

	//
	// This .dds loader supports the loading of compressed formats DXT1, DXT3 
	// and DXT5.
	//

	DDSImageData.encoding = ddsd.ddpfPixelFormat.dwFourCC;
	switch( DDSImageData.encoding )
	{
		case FOURCC_DXT1:
			// DXT1's compression ratio is 8:1
			DDSImageData.internalformat = srgb ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT : GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			DDSImageData.components = 3;
			DDSImageData.bitsPerPixel = 4;
			DDSImageData.bIsCompressed = true;
			DDSImageData.bIsFloatingPoint = false;
			mipmapFactor = 2;
			break;
		case FOURCC_DXT3:
			// DXT3's compression ratio is 4:1
			DDSImageData.internalformat = srgb ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT : GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			DDSImageData.components = 4;
			DDSImageData.bitsPerPixel = 8;
			DDSImageData.bIsCompressed = true;
			DDSImageData.bIsFloatingPoint = false;
			mipmapFactor = 4;
			break;
		case FOURCC_DXT5:
			// DXT5's compression ratio is 4:1
			DDSImageData.internalformat =  srgb ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			DDSImageData.components = 4;
			DDSImageData.bitsPerPixel = 8;
			DDSImageData.bIsCompressed = true;
			DDSImageData.bIsFloatingPoint = false;
			mipmapFactor = 4;
			break;
		case D3DFMT_R16F:
			DDSImageData.internalformat = GL_R16F;
			DDSImageData.components = 1;
			DDSImageData.bitsPerPixel = 16;
			DDSImageData.bIsCompressed = false;
			DDSImageData.bIsFloatingPoint = true;
			mipmapFactor = 2;
			break;
		case D3DFMT_A16B16G16R16F:
			DDSImageData.internalformat = GL_RGBA16F;
			DDSImageData.components = 4;
			DDSImageData.bitsPerPixel = 64;
			DDSImageData.bIsCompressed = false;
			DDSImageData.bIsFloatingPoint = true;
			mipmapFactor = 2;
			break;
		default:
			ConsoleMessage( "The file \"%s\" doesn't appear to be compressed "\
				"using DXT1, DXT3, DXT5 or R16F", pFilename );
			return false;
	}

	//
	// How big will the buffer need to be to load all of the pixel data 
	// including mip-maps?
	//

	if( ddsd.dwLinearSize == 0 )
	{
		ConsoleMessage( "dwLinearSize is 0!" );
		return false;
	}

	DDSImageData.width      = ddsd.dwWidth;
	DDSImageData.height     = ddsd.dwHeight;
	DDSImageData.bHasMipMaps = (ddsd.dwFlags & DDSD_MIPMAPCOUNT) != 0;

	if( DDSImageData.bHasMipMaps )
		DDSImageData.numMipMaps = ddsd.dwMipMapCount;
	else
		DDSImageData.numMipMaps = 0;

	if( DDSImageData.numMipMaps > 0 )
		bufferSize = ddsd.dwLinearSize * mipmapFactor;
	else
		bufferSize = ddsd.dwLinearSize;

	DDSImageData.pixels.resize(bufferSize);

	is.read( (char*)DDSImageData.pixels.data(), bufferSize );

	// Close the file
	is.close();


	//Flip the image for OpenGL
	//Do the flip if flipY is false, because DDS Files are always flipped upside down
	if( !flipY ) 
		FlipImage( DDSImageData );

	////////////////////////////////////
	//Load texture into graphics mem
	////////////////////////////////////
	LoadIntoGpu(DDSImageData, textureTarget, index);

	g_LoadingMutex->Release();

	//copy data into texture_t object
	tex.filename = pFilename;
	tex.index = index;
	tex.height = DDSImageData.height;
	tex.width = DDSImageData.width;

	return true;
}






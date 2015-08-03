// filesystem/zip.cpp
// filesystem/zip.cpp
// filesystem/zip.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "zip.h"
#include "util.h"
#include <limits>

namespace filesystem
{
namespace zip
{

const uint32_t eocd_t::needed_signature = 0x06054b50;
const uint32_t cdh_t::needed_signature = 0x02014b50;
const uint32_t lfh_t::needed_signature = 0x04034b50;



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool eocd_t::deserialize( std::istream &is )
{
	is.seekg(0, std::ios::end);
	std::streampos spSize = is.tellg();
	if(spSize > std::numeric_limits<uint32_t>::max())
		return false;
	uint32_t archiveSize = (uint32_t)spSize;
	if(archiveSize < 22)
		return false;

	//Try to find eocd signature
	for(uint32_t i = 22; i <= 22 + (uint32_t)std::numeric_limits<uint16_t>::max() && i <= archiveSize; ++i)
	{
		is.seekg(-int32_t(i), std::ios::end);
		signature = 0;
		deserializeBinary(is, signature);
		if(signature == needed_signature)
		{
			//deserialize following members
			deserializeBinary(is, numberOfDisk);	//Number of this disk
			deserializeBinary(is, diskOfCD);		//Disk where cd starts
			deserializeBinary(is, numOfDiskCDs);	//Number of central directory records on this disk
			deserializeBinary(is, numOfCDs);		//Total number of central directory records
			deserializeBinary(is, sizeOfCD);		//Size of central directory (bytes)
			deserializeBinary(is, offsetOfCD);		//Offset of central directory, relative to start of archive
			deserializeBinary(is, commentLength);	//Length of comment
			if(!is.good())
				return false;
			if(commentLength == i - 22)
			{
				//read comment
				comment.resize(commentLength);
				if(commentLength > 0)
					is.read(&comment[0], commentLength);

				//Validate other members:
				return numberOfDisk == 0 && diskOfCD == 0 && numOfCDs == numOfDiskCDs;				
			}
		}
	}

	return false; //limit reached when searching for cd?
}




//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool cdh_t::deserialize( std::istream &is )
{
	//read signature
	signature = 0;
	deserializeBinary(is, signature);
	if( signature != needed_signature )
		return false;

	//read rest of header
	deserializeBinary(is, versionMadeBy);		//Version made by
	deserializeBinary(is, versionNeeded);		//Version needed to extract
	deserializeBinary(is, bitFlags);			//general purpose bit flags
	deserializeBinary(is, comprMethod);			//compression method
	deserializeBinary(is, fileLastModTime);		//File last modification time
	deserializeBinary(is, fileLastModDate);		//File last modification date
	deserializeBinary(is, crc32);				//CRC-32
	deserializeBinary(is, sizeCompressed);		//compressed size
	deserializeBinary(is, sizeUncompressed);	//uncompressed size
	deserializeBinary(is, lengthFileName);		//length of filename
	deserializeBinary(is, lengthExtraField);	//length of extra field
	deserializeBinary(is, lengthFileComment);	//File comment length
	deserializeBinary(is, diskNrOfFile);		//Disk number where file starts
	deserializeBinary(is, intFileAttr);			//internal file attributes
	deserializeBinary(is, extFileAttr);			//external file attributes
	deserializeBinary(is, fileOffset);			//Relative offset of local file header (bytes between start of first disk of file and the local file header)
	if(!is.good())
		return false;

	if(comprMethod != 0 || sizeCompressed != sizeUncompressed || diskNrOfFile != 0)
		return false;

	fileName.resize(lengthFileName);
	if(lengthFileName > 0)
		is.read(&fileName[0], lengthFileName);
	if(!is.good())
		return false;

	extra.resize(lengthExtraField);
	if(lengthExtraField > 0)
		is.read(&extra[0], lengthExtraField);
	if(!is.good())
		return false;

	fileComment.resize(lengthFileComment);
	if(lengthFileComment > 0)
		is.read(&fileComment[0], lengthFileComment);
	if(!is.good())
		return false;

	return true;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool lfh_t::deserialize( std::istream &is, const cdh_t &cdh )
{
	//read signature
	signature = 0;
	deserializeBinary(is, signature);
	if(signature != needed_signature)
		return false;

	//read rest of header
	deserializeBinary(is, versionNeeded);		//Version needed to extract
	deserializeBinary(is, bitFlags);			//general purpose bit flags
	deserializeBinary(is, comprMethod);			//compression method
	deserializeBinary(is, fileLastModTime);		//File last modification time
	deserializeBinary(is, fileLastModDate);		//File last modification date
	deserializeBinary(is, crc32);				//CRC-32
	deserializeBinary(is, sizeCompressed);		//compressed size
	deserializeBinary(is, sizeUncompressed);	//uncompressed size
	deserializeBinary(is, lengthFileName);		//length of filename
	deserializeBinary(is, lengthExtraField);	//length of extra field
	if(!is.good())
		return false;

	//validate against central directory header
	if( versionNeeded != cdh.versionNeeded ||
		comprMethod != 0 || 
		lengthFileName != cdh.lengthFileName ||
		lengthExtraField != cdh.lengthExtraField ||
		(!(bitFlags & 0x08) && (sizeCompressed != cdh.sizeCompressed || 
								sizeUncompressed != cdh.sizeUncompressed)) ||
		sizeCompressed != sizeUncompressed )
		return false;

	fileName.resize(lengthFileName);
	if(lengthFileName > 0)
		is.read(&fileName[0], lengthFileName);
	if(!is.good())
		return false;

	extra.resize(lengthExtraField);
	if(lengthExtraField > 0)
		is.read(&extra[0], lengthExtraField);
	if(!is.good())
		return false;

	fileStart = (uint32_t)is.tellg();

	return true;
}

}
}

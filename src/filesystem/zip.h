// filesystem/zip.h
// filesystem/zip.h
// filesystem/zip.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once

#include <stdint.h>
#include <istream>
#include <vector>

namespace filesystem
{
namespace zip
{
	
//end of central directory structure = 0x06054b50
struct eocd_t
{
	bool deserialize(std::istream &is);
	static const uint32_t needed_signature;

	uint32_t signature;		//End of central directory signature
	uint16_t numberOfDisk;	//Number of this disk
	uint16_t diskOfCD;		//Disk where cd starts
	uint16_t numOfDiskCDs;	//Number of central directory records on this disk
	uint16_t numOfCDs;		//Total number of central directory records
	uint32_t sizeOfCD;		//Size of central directory (bytes)
	uint32_t offsetOfCD;	//Offset of central directory, relative to start of archive
	uint16_t commentLength; //Length of comment
	std::vector<char> comment;
};


//central directory file header
struct cdh_t
{
	bool deserialize(std::istream &is);
	static const uint32_t needed_signature;

	uint32_t signature;			//Central directory file header signature = 0x02014b50
	uint16_t versionMadeBy;		//Version made by
	uint16_t versionNeeded;		//Version needed to extract
	uint16_t bitFlags;			//general purpose bit flags
	uint16_t comprMethod;		//compression method
	uint16_t fileLastModTime;	//File last modification time
	uint16_t fileLastModDate;	//File last modification date
	uint32_t crc32;				//CRC-32
	uint32_t sizeCompressed;	//compressed size
	uint32_t sizeUncompressed;  //uncompressed size
	uint16_t lengthFileName;	//length of filename
	uint16_t lengthExtraField;	//length of extra field
	uint16_t lengthFileComment; //File comment length
	uint16_t diskNrOfFile;		//Disk number where file starts
	uint16_t intFileAttr;		//internal file attributes
	uint32_t extFileAttr;		//external file attributes
	uint32_t fileOffset;		//Relative offset of local file header (bytes between start of first disk of file and the local file header)
	std::vector<char> fileName;
	std::vector<char> extra;
	std::vector<char> fileComment;
};


////data descriptor
//struct data_desc
//{
//	bool deserialize(std::istream &is);
//	static const uint32_t needed_signature;
//
//	uint32_t signature;			//Data descriptor signature (optional)
//	uint32_t crc32;				//CRC-32
//	uint32_t sizeCompressed;	//compressed size
//	uint32_t sizeUncompressed;	//uncompressed size
//};

//local file header
struct lfh_t
{
	bool deserialize(std::istream &is, const cdh_t &cdh);
	static const uint32_t needed_signature;

	uint32_t signature;			//Central directory file header signature = 0x02014b50
	uint16_t versionNeeded;		//Version needed to extract
	uint16_t bitFlags;			//general purpose bit flags
	uint16_t comprMethod;		//compression method
	uint16_t fileLastModTime;	//File last modification time
	uint16_t fileLastModDate;	//File last modification date
	uint32_t crc32;				//CRC-32
	uint32_t sizeCompressed;	//compressed size
	uint32_t sizeUncompressed;  //uncompressed size
	uint16_t lengthFileName;	//length of filename
	uint16_t lengthExtraField;	//length of extra field
	std::vector<char> fileName; //file name
	std::vector<char> extra;	//extra field

	uint32_t fileStart;			//NOT part of standard, just for our application
};

}
}


// filesystem/instream.h
// filesystem/instream.h
// filesystem/instream.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once

#include <istream>
#include "archiveStreamBuf.h"

namespace filesystem
{

template<class _Elem, class _Traits>
class basic_InStream : public std::basic_istream<_Elem, _Traits>
{
public:
	typedef basic_InStream<_Elem, _Traits> MyT;
	typedef std::basic_istream<_Elem, _Traits> MybT;
	typedef ArchiveStreamBuf<_Elem, _Traits> MyFbT;
	typedef std::basic_ios<_Elem, _Traits> MyIosT;

	basic_InStream() :
		MybT(&m_fb)
	{

	}

	explicit basic_InStream(const char *filename,
		std::ios_base::openmode mode = std::ios_base::in,
		Filesystem *fs = NULL) :
		m_fb(fs),
		MybT(&m_fb)
	{
		if(m_fb.open(filename, mode | std::ios_base::in) == 0)
			MyIosT::setstate(std::ios_base::failbit);
	}

	explicit basic_InStream(const std::string &filename,
		std::ios_base::openmode mode = std::ios_base::in) :
		MybT(&m_fb)
	{
		if(m_fb.open(filename.c_str(), mode | std::ios_base::in) == 0)
			MyIosT::setstate(std::ios_base::failbit);
	}

private:
	basic_InStream(const MyT &other); //not defined
	MyT &operator=(const MyT &other); //not defined

public:
	void open(const char *filename,
		std::ios_base::openmode mode = std::ios_base::in)
	{
		if(m_fb.open(filename, mode | std::ios_base::in) == 0)
			MyIosT::setstate(std::ios_base::failbit);
		else
			MyIosT::clear(); //added with c++11
	}

	void open(const std::string &filename,
		std::ios_base::openmode mode = std::ios_base::in)
	{
		open(filename.c_str(), mode);
	}

	//void open(const wchar_t *filename,
	//	std::ios_base::openmode = std::ios_base::in)
	//{
	//	if(m_fb.open(filename, mode | std::ios_base::in) == 0)
	//		MyIosT::setstate(std::ios_base::failbit);
	//	else
	//		MyIosT::clear(); //added with c++11
	//}

	//void open(const std::wstring &filename,
	//	std::ios_base::openmode mode = std::ios_base::in)
	//{
	//	open(filename.c_str(), mode);
	//}

	void close()
	{
		if(m_fb.close() == 0)
			MyIosT::setstate(std::ios_base::failbit);
	}

	bool is_open() const
	{
		return m_fb.is_open();
	}

	MyFbT *rdbuf() const
	{
		return &m_fb;
	}

	size_t getFileSize() const
	{
		return m_fb.getFileSize();
	}

private:
	MyFbT m_fb;
};

typedef basic_InStream<char, std::char_traits<char>> InStream;

}


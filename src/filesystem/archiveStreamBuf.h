// filesystem/archiveStreamBuf.h
// filesystem/archiveStreamBuf.h
// filesystem/archiveStreamBuf.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once

#include <fstream>
#include <assert.h>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include "filesystem.h"


namespace filesystem
{

//Currently only works with _Elem = char!
template<class _Elem, class _Traits>
class ArchiveStreamBuf : public std::basic_streambuf<_Elem, _Traits>
{
private:
	//returns false if file was not found in any mounted archive
	bool getFileArchiveInfo(const char *path, fileinfo_t &info)
	{
		return m_fs->findFile(path, &info);
	}

private:
	typedef ArchiveStreamBuf<_Elem, _Traits> MyT;
	typedef std::basic_streambuf<_Elem, _Traits> MybT;

public:
	ArchiveStreamBuf(Filesystem *fs = NULL) :
		m_file(NULL),
		m_buf(1024),
		m_fs(fs),
		m_fileStart(0),
		m_fileSize(0)
	{
		if(!m_fs)
			m_fs = &Filesystem::GetInstance();
	}

	~ArchiveStreamBuf()
	{
		close();
	}

	MyT *open(const char *filename, std::ios_base::openmode mode)
	{
		if((mode & std::ios_base::out) || !(mode & std::ios_base::in))
			throw std::logic_error("Only input supported!");

		//file exists locally?
		m_file = fopen(filename, "rb");
		if(m_file)
		{
			m_fileStart = 0;
			fseek(m_file, 0, SEEK_END);
			m_fileSize = ftell(m_file);
			fseek(m_file, 0, SEEK_SET);
		}
		else //Search archives
		{
			fileinfo_t info;
			if(getFileArchiveInfo(filename, info) && (m_file = fopen(info.archiveName, "rb")))
			{
				m_fileStart = info.fileStart;
				m_fileSize = info.fileSize;
				fseek(m_file, m_fileStart, SEEK_SET);
			}
		}

		if(m_file)
		{
			_Elem *end = &m_buf[0] + m_buf.size() * sizeof(_Elem);
			MybT::setg(end, end, end); //invalidate pointers
			return this;
		}

		return NULL;
	}

	MyT *open(const std::string &filename, std::ios_base::openmode mode)
	{
		return open(filename.c_str(), mode);
	}

	MyT *close()
	{
		if(!m_file)
			return NULL;
		fclose(m_file);
		m_file = NULL;
		return this;
	}

	bool is_open() const
	{
		return m_file != NULL;
	}

	size_t getFileSize() const
	{
		return (size_t)m_fileSize;
	}

private:
	ArchiveStreamBuf(const MyT &other); //not defined
	MyT &operator=(const MyT&); //not defined

protected:
	virtual typename MybT::int_type underflow() override
	{
		assert(MybT::gptr() >= MybT::eback() && MybT::gptr() <= MybT::egptr());
		if(MybT::gptr() < MybT::egptr())
			return _Traits::to_int_type(*MybT::gptr());
		int pos = ftell(m_file) - m_fileStart;
		int remaining = m_fileSize - pos;
		if(remaining <= 0)
			return _Traits::eof();
		_Elem *start = &m_buf[0];
		size_t n = fread(start, 1, std::min((int)m_buf.size(), remaining), m_file);
		assert(n > 0);
		MybT::setg(start, start, start + n);
		return _Traits::to_int_type(*MybT::gptr());
	}

	virtual typename MybT::int_type uflow() override
	{
		if ( underflow() == _Traits::eof() ) 
			return _Traits::eof();
		MybT::gbump(1);
		return MybT::gptr()[-1];
	}

	virtual typename MybT::pos_type seekoff(typename MybT::off_type off,
		std::ios_base::seekdir way,
		std::ios_base::openmode mode = std::ios_base::in) override
	{
		assert(MybT::gptr() >= MybT::eback() && MybT::gptr() <= MybT::egptr());
		if((int)off < 0)
            return std::streampos(std::streamoff(-1));
		int currPos = (ftell(m_file) - m_fileStart) - (size_t)(MybT::egptr() - MybT::gptr());
		int absPos;
		switch(way)
		{
		case std::ios_base::beg:
			absPos = (int)off; break;
		case std::ios_base::cur:
			absPos = currPos + (int)off; break;
		case std::ios_base::end:
			absPos = m_fileSize + (int)off; break;
		default:
			return std::streampos(std::streamoff(-1));
		}

		if(absPos == currPos)
			return std::streampos(currPos);

		return seekpos(absPos, mode);
	}

	virtual typename MybT::pos_type seekpos(typename MybT::pos_type pos,
		std::ios_base::openmode mode = std::ios_base::in) override
	{
		assert(MybT::gptr() >= MybT::eback() && MybT::gptr() <= MybT::egptr());
		MybT::setg(MybT::egptr(), MybT::egptr(), MybT::egptr()); //invalidate pointers
		if((int)pos < 0 || (int)pos > m_fileSize)
			return std::streampos(std::streamoff(-1));
		int res = fseek(m_file, (int)pos + m_fileStart, SEEK_SET);
		return res == 0 ? pos : std::streampos(std::streamoff(-1));
	}

	virtual std::streamsize xsgetn(_Elem *ptr, std::streamsize count) override
	{
		assert(MybT::gptr() >= MybT::eback() && MybT::gptr() <= MybT::egptr());
		if((int)count < 0)
			return std::streamsize(-1);

		//first copy from our buffer
		size_t remaining = (size_t)(MybT::gptr() - MybT::eback());
		size_t read = 0;
		if(remaining > 0)
		{
			read = std::min(remaining, (size_t)count);
			std::memcpy(ptr, MybT::gptr(), read * sizeof(_Elem));
		}
		if((size_t)count > remaining) //if our buffer is depleted call fread to read the rest
		{
			read += fread(ptr + read * sizeof(_Elem), sizeof(_Elem), (size_t)count - read, m_file);
			MybT::setg(MybT::egptr(), MybT::egptr(), MybT::egptr()); //invalidate pointers
		}
		return std::streamsize(read);
	}

	virtual std::streamsize showmanyc() override
	{
		return 0;
	}

	//Unimplemented functions:
	virtual typename MybT::int_type pbackfail(typename MybT::int_type meta = _Traits::eof()) override {
		throw std::logic_error("not supported.");
	}
	virtual MybT *setbuf(_Elem *buffer, std::streamsize count) override {
		throw std::logic_error("not supported.");
	}
	virtual typename MybT::int_type sync() override {
		throw std::logic_error("not supported");
	}
	virtual void imbue(const std::locale &loc) override {
		//m_fb.pubimbue(loc);
	}
	virtual std::streamsize xsputn(const _Elem *ptr, std::streamsize count) override {
		throw std::logic_error("not supported");
	}
	virtual typename MybT::int_type overflow(typename MybT::int_type meta = _Traits::eof()) override {
		throw std::logic_error("not supported");
	}

private:
	FILE *m_file;
	std::vector<_Elem> m_buf;
	Filesystem *m_fs;
	int m_fileStart;
	int m_fileSize;
};

}

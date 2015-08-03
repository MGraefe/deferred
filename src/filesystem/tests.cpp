// filesystem/tests.cpp
// filesystem/tests.cpp
// filesystem/tests.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

//#define COMPILE_TESTS
#ifdef COMPILE_TESTS

#include "folder.h"
#include "file.h"
#include "filesystem_impl.h"
#include <stack>
#include <iostream>
#include "instream.h"

bool listFiles(const filesystem::Folder &folder, std::string &ident)
{
	std::cout << ident << "Folder " << folder.getName() << ": " << std::endl;
	ident += "  ";

	for(const filesystem::File &file : folder.getFiles())
		std::cout << ident << "File " << file.getName() << std::endl;

	for(const filesystem::Folder &subfolder : folder.getFolders())
		listFiles(subfolder, ident);

	ident.erase(ident.length()-2, 2);
	std::cout << ident << "END Folder " << folder.getName() << std::endl;
	return true;
}


bool largeFileTest()
{
	system("PAUSE");
	{
		std::cout << std::endl <<  "---------------------------------------------------------" << std::endl;
		std::cout << "getline test:" << std::endl << std::endl;
		filesystem::InStream is;
		is.open("C:/Users/RedPuma/Desktop/ziptest/readme.txt");
		char line[1024];
		while(is.good())
		{
			is.getline(line, 1024);
			std::cout << line << std::endl;
		}
	}
	system("PAUSE");
	{
		std::cout << std::endl <<  "---------------------------------------------------------" << std::endl;
		std::cout << "read test:" << std::endl << std::endl;
		filesystem::InStream is;
		is.open("C:/Users/RedPuma/Desktop/ziptest/readme.txt");
		is.seekg(0, std::ios::end);
		size_t size = (size_t)is.tellg();
		char *buf = new char[size+1];
		is.seekg(0, std::ios::beg);
		is.read(buf, size);
		buf[size] = 0;
		std::cout << buf << std::endl;
	}

	return true;
}


bool listFiles(std::string filename)
{
	if(!filesystem::Filesystem_impl::GetInstance().mountArchive(filename.c_str()))
		return false;

	const filesystem::Folder *root = filesystem::Filesystem_impl::GetInstance().getArchives().front()->getRoot();
	listFiles(*root, std::string());

	filesystem::InStream inStream("folder1/asd3.txt", std::ios::beg);
	inStream.seekg(0, std::ios::end);
	int filesize = (int)inStream.tellg();
	std::cout << "folder1/asd3.txt size: " << (int)inStream.tellg() << std::endl;
	std::cout << "folder1/asd3.txt content: " << std::endl;

	//Reading file in first approach
	char *content = new char[filesize+1];
	inStream.seekg(0, std::ios::beg);
	inStream.read(content, filesize);
	content[filesize] = 0;
	std::cout << content << std::endl;
	delete[] content;

	//Reading file in second approach
	inStream.seekg(0, std::ios::beg);
	std::string content2;
	while(inStream.good())
	{
		char ch = '@';
		inStream.get(ch);
		content2 += ch;
	}
	std::cout << "Content on second approach: " << std::endl;
	std::cout << content2 << std::endl;
	inStream.close();

	largeFileTest();

	return true;
};


#ifdef _WIN32

#include <Windows.h>
#include <fcntl.h>
#include <stdio.h>
#include <io.h>


void InitConsole()
{
	AllocConsole();
	SetConsoleTitleA("Filesystem tests");

	int hConHandle;
	FILE *fp;

	// redirect unbuffered STDOUT to the console
	HANDLE __hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle((long)__hStdOut, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	std::cout.clear();

	// redirect unbuffered STDERR to the console
	HANDLE __hStdError = GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle((long)__hStdError, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
	std::cerr.clear();

	//DebugBreak();
}


extern "C" void __declspec(dllexport) CALLBACK RunTests(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	InitConsole();

	std::cout << "cmd-line: " << lpszCmdLine << std::endl;
	std::cout << "Running test listFiles on file " << lpszCmdLine << ":" << std::endl;
	listFiles(lpszCmdLine);
	system("PAUSE");
}

#pragma comment(linker, "/EXPORT:RunTests=_RunTests@16")

#endif //_WIN32

#endif //COMPILE_TESTS
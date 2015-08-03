// util/MessageBox.cpp
// util/MessageBox.cpp
// util/MessageBox.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#ifndef _WINDOWS

#include "MessageBox.h"
#include <sstream>
#include <string>
#include <stdlib.h>

using std::string;

string quoteString(std::string text)
{
	string quote;
	quote.reserve(text.length());
	quote.push_back('\"');
	for(size_t i = 0; i < text.length(); i++)
	{
		if(text[i] == '\\' || text[i] == '\"')
			quote.push_back('\\');
		quote.push_back(text[i]);
	}
	quote.push_back('\"');
	return quote;
}

int MessageBoxA(void *owner /*= NULL*/, const char *text /*= NULL*/, const char *caption /*= NULL*/, int type /*= MB_OK*/)
{
	string buttons;
	if(type & MB_YESNO)
		buttons = "Yes:6,No:7 -default Yes";
	else
		buttons = "OK:0 -default OK";

	std::string _caption = caption ? quoteString(caption) : "\"Error\"";
	std::string _text = text ? quoteString(text) : _caption;

	std::ostringstream command;
	command << "xmessage -buttons " << buttons
			<< " -center"
			<< " -title " << _caption
			<< " -name " << _caption
			<< " " << _text;

	int ret = system(command.str().c_str());
	if(ret == -1 || ret == 127)
		return -1;
	else
		return ret;
}

#endif


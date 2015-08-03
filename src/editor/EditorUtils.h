// editor/EditorUtils.h
// editor/EditorUtils.h
// editor/EditorUtils.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

//EditorUtils.h

#pragma once
#ifndef deferred__editor__EditorUtils_H__
#define deferred__editor__EditorUtils_H__

#include <vector>

class CWnd;
namespace util
{
	std::vector<CWnd*> GetChildWindows(CWnd *parent);
}

#endif // deferred__editor__EditorUtils_H__
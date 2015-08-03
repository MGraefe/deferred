// editor/EditorUtils.cpp
// editor/EditorUtils.cpp
// editor/EditorUtils.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "EditorUtils.h"


namespace util
{
	BOOL CALLBACK GetChildWindows_CallbackCWnd(_In_ HWND hwnd, _In_ LPARAM lParam)
	{
		std::vector<CWnd*> *vec = (std::vector<CWnd*>*)lParam;
		vec->push_back(CWnd::FromHandle(hwnd));
		return TRUE;
	}


	std::vector<CWnd*> GetChildWindows(CWnd *parent)
	{
		std::vector<CWnd*> vec;
		EnumChildWindows(parent->GetSafeHwnd(), &GetChildWindows_CallbackCWnd, (LPARAM)&vec);
		return vec;
	}


	BOOL CALLBACK GetChildWindows_CallbackHWND(_In_ HWND hwnd, _In_ LPARAM lParam)
	{
		std::vector<HWND> *vec = (std::vector<HWND>*)lParam;
		vec->push_back(hwnd);
		return TRUE;
	}


	std::vector<HWND> GetChildWindows(HWND parent)
	{
		std::vector<HWND> vec;
		EnumChildWindows(parent, &GetChildWindows_CallbackHWND, (LPARAM)&vec);
		return vec;
	}
}
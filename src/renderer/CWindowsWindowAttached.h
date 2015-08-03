// renderer/CWindowsWindowAttached.h
// renderer/CWindowsWindowAttached.h
// renderer/CWindowsWindowAttached.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CWindowsWindowAttached_H__
#define deferred__renderer__CWindowsWindowAttached_H__

#ifdef _WINDOWS

#include "IWindow.h"

class CWindowsWindowAttached : public IWindow
{
public:
	CWindowsWindowAttached();
	bool Create(CWindowCreateInfo *info);
	void Kill();
	void HandleMessages(bool &quitMessage);
	//bool bindRenderContext();
	bool HasSecondContext();
	bool BindRenderingContext(UINT index);
	bool UnBindRenderingContext(UINT index);
	bool SwapBuffers();

	int GetWidth();
	int GetHeight();
	bool GetCursorPos(int &x, int &y);
	bool SetCursorPos(int x, int y);

	bool IsActive();
	bool IsFullscreen();

	void *GetWindowSystemReference();

	bool Attach( HDC hDC, HGLRC hlRC, HWND hWnd);


private:
	//HINSTANCE m_hInstance;
	HWND m_hWnd;
	HDC m_hDC;
	HGLRC m_hRC[2];
};

#endif /* _WINDOWS */
#endif /* CWINDOWSWINDOWATTACHED_H */
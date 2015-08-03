// renderer/CWindowsWindow.h
// renderer/CWindowsWindow.h
// renderer/CWindowsWindow.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CWindowsWindow_H__
#define deferred__renderer__CWindowsWindow_H__

#include "IWindow.h"

#ifdef _WINDOWS

class CWindowsWindow : public IWindow
{
public:
	CWindowsWindow();
	~CWindowsWindow();
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


	static bool GetCursorPos(HWND hWnd, int &x, int &y);
	static bool SetCursorPos(HWND hWnd, int x, int y);

private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	HDC m_hDC;
	HGLRC m_hRC[2];
	GLuint m_pixelFormat;

	bool m_bFullscreen;
};

#endif

#endif

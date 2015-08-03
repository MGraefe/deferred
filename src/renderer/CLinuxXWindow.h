// renderer/CLinuxXWindow.h
// renderer/CLinuxXWindow.h
// renderer/CLinuxXWindow.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CLinuxXWindow_H__
#define deferred__renderer__CLinuxXWindow_H__

#include "IWindow.h"

#ifdef _LINUX

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glx.h>

class CLinuxXWindow : public IWindow
{
public:
	CLinuxXWindow();
	~CLinuxXWindow();
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

private:
	Display *m_dpy;
	Window m_win;
	GLXContext m_glc[2];
	Atom m_wmDeleteMessage;
};

#endif /* _LINUX */
#endif /* CLINUXXWINDOW_H_ */

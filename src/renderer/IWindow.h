// renderer/IWindow.h
// renderer/IWindow.h
// renderer/IWindow.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__IWindow_H__
#define deferred__renderer__IWindow_H__

class CWindowCreateInfo;

class IWindow
{
public:
	virtual ~IWindow() {}
	virtual bool Create(CWindowCreateInfo *info) = 0;
	virtual void Kill() = 0;
	virtual void HandleMessages(bool &quitMessage) = 0;
	//bool bindRenderContext();
	virtual bool HasSecondContext() = 0;
	virtual bool BindRenderingContext(UINT index) = 0;
	virtual bool UnBindRenderingContext(UINT index) = 0;
	virtual bool SwapBuffers() = 0; 
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
	virtual bool GetCursorPos(int &x, int &y) = 0;
	virtual bool SetCursorPos(int x, int y) = 0;
	virtual bool IsActive() = 0;
	virtual bool IsFullscreen() = 0;
	virtual void *GetWindowSystemReference() = 0;
};

#endif

// renderer/CLinuxXWindow.cpp
// renderer/CLinuxXWindow.cpp
// renderer/CLinuxXWindow.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "CLinuxXWindow.h"
#include "CWindowCreateInfo.h"

#ifdef _LINUX

CLinuxXWindow::CLinuxXWindow()
{
	m_dpy = NULL;
	m_win = 0;
	m_glc[0] = NULL;
	m_glc[1] = NULL;
	m_wmDeleteMessage = 0;
}


CLinuxXWindow::~CLinuxXWindow()
{

}


bool CLinuxXWindow::Create(CWindowCreateInfo *info)
{
	int att[] = { 	/*GLX_BUFFER_SIZE, info->colorBits + info->alphaBits,*/
					GLX_RGBA,
					GLX_DEPTH_SIZE, info->depthBits,
					GLX_DOUBLEBUFFER, None };

	Window 					root;
	XVisualInfo 			*vi;
	Colormap 				cmap;
	XSetWindowAttributes 	swa;

	m_dpy = XOpenDisplay(NULL);
	if( !m_dpy )
	{
		error("Could not connect to X-Server.");
		return false;
	}

	root = DefaultRootWindow(m_dpy);
	vi = glXChooseVisual(m_dpy, 0, att);

	if( !vi )
	{
		error("No appropriate visual info found");
		return false;
	}
	else
		ConsoleMessage("Selected Visual-ID: %p", (void*)vi->visualid);

	cmap = XCreateColormap(m_dpy, root, vi->visual, AllocNone);

	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

	m_win = XCreateWindow(
			m_dpy,						//display
			root,						//parent
			0, 0,						//x, y
			info->width, info->height,	//width, height
			0,							//border width
			vi->depth,					//depth
			InputOutput,				//window class
			vi->visual,					//visuals
			CWColormap | CWEventMask,	//value masks
			&swa);						//set window attributes

	XMapWindow(m_dpy, m_win);
	XStoreName(m_dpy, m_win, info->title);

	m_glc[0] = glXCreateContext(m_dpy, vi, NULL, GL_TRUE);
	if( !m_glc[0] )
	{
		error("Error creating the first opengl context");
		return false;
	}

	m_glc[1] = glXCreateContext(m_dpy, vi, m_glc[0], GL_TRUE);
	if( !m_glc[1] )
		error("Error creating the second opengl context");

	m_wmDeleteMessage = XInternAtom(m_dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(m_dpy, m_win, &m_wmDeleteMessage, 1);

	ConsoleMessage("Created X Window with id %i on display %p", m_win, m_dpy);

	return true;
}


void CLinuxXWindow::Kill()
{
	if( !m_dpy ) //Already killed?
		return;

	ConsoleMessage("Unbinding context");
	if( !glXMakeCurrent(m_dpy, None, NULL) )
		error("Releasing opengl context failed");

	if( m_glc[1] )
	{
		ConsoleMessage("Destroying context 1");
		glXDestroyContext(m_dpy, m_glc[1]);
		m_glc[1] = NULL;
	}

	if( m_glc[0] )
	{
		ConsoleMessage("Destroying context 0");
		glXDestroyContext(m_dpy, m_glc[0]);
		m_glc[0] = NULL;
	}

	if(m_win != 0)
	{
		ConsoleMessage("Destroying Window");
		XDestroyWindow(m_dpy, m_win);
	}

	ConsoleMessage("Closing Display");
	XCloseDisplay(m_dpy);

	m_win = 0;
	m_dpy = NULL;
}


void CLinuxXWindow::HandleMessages(bool &quitMessage)
{
	return;
	XEvent xev;

	while(XPending(m_dpy) > 0)
	{
		XNextEvent(m_dpy, &xev);
		if(xev.type == ClientMessage && (Atom)xev.xclient.data.l[0] == m_wmDeleteMessage)
		{
			//XDestroyWindow(m_dpy, xev.xclient.window);
			//m_win = 0;
			quitMessage = true;
			return;
		}
	}
}


bool CLinuxXWindow::HasSecondContext()
{
	return m_glc[1] != NULL;
}


bool CLinuxXWindow::BindRenderingContext(UINT index)
{
	if(index > 2 || m_glc[index] == NULL)
	{
		error_fatal("No device context for index %i", index);
		return false;
	}

	return glXMakeCurrent(m_dpy, m_win, m_glc[index]) == True;
}


bool CLinuxXWindow::UnBindRenderingContext(UINT index)
{
	ConsoleMessage("Unbinding Rendering context %i", index);
	return glXMakeCurrent(m_dpy, None, NULL);
}


bool CLinuxXWindow::SwapBuffers()
{
	if(m_dpy && m_win != 0)
		glXSwapBuffers(m_dpy, m_win);
	return true;
}


int CLinuxXWindow::GetWidth()
{
	XWindowAttributes wa;
	XGetWindowAttributes(m_dpy, m_win, &wa);
	return wa.width;
}


int CLinuxXWindow::GetHeight()
{
	XWindowAttributes wa;
	XGetWindowAttributes(m_dpy, m_win, &wa);
	return wa.height;
}


bool CLinuxXWindow::GetCursorPos(int &x, int &y)
{
	return true;
}


bool CLinuxXWindow::SetCursorPos(int x, int y)
{
	return true;
}


bool CLinuxXWindow::IsActive()
{
	return true;
}


bool CLinuxXWindow::IsFullscreen()
{
	return false;
}


void *CLinuxXWindow::GetWindowSystemReference()
{
	return (void*)m_win;
}

#endif /* _LINUX */

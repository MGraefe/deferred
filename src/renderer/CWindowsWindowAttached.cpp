// renderer/CWindowsWindowAttached.cpp
// renderer/CWindowsWindowAttached.cpp
// renderer/CWindowsWindowAttached.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#ifdef _WINDOWS

#include "CWindowsWindow.h"
#include "CWindowsWindowAttached.h"
#include "CWindowCreateInfo.h"


CWindowsWindowAttached::CWindowsWindowAttached()
{
	//m_hInstance = 0;
	m_hWnd = 0;
	m_hDC = 0;
	m_hRC[0] = NULL;
	m_hRC[1] = NULL;
}

bool CWindowsWindowAttached::Attach( HDC hDC, HGLRC hlRC, HWND hWnd)
{
	//m_hInstance = NULL;
	m_hDC = hDC;
	m_hRC[0] = hlRC;
	m_hRC[1] = NULL;
	m_hWnd = hWnd;
	return TRUE;
}

bool CWindowsWindowAttached::Create(CWindowCreateInfo *info)
{
	throw std::exception();
}

void CWindowsWindowAttached::Kill()
{
	throw std::exception();
}

void CWindowsWindowAttached::HandleMessages(bool &quitMessage)
{
	throw std::exception();
}

//bool bindRenderContext();
bool CWindowsWindowAttached::HasSecondContext()
{
	return m_hRC[1] != NULL;
}

bool CWindowsWindowAttached::BindRenderingContext(UINT index)
{
	if(index > 2 || m_hRC[index] == NULL)
	{
		error_fatal("No device context for index %i", index);
		return false;
	}

	if( !wglMakeCurrent( m_hDC, m_hRC[index] ) )
	{
		error_fatal( "wglMakeCurrent: Failed to bind the rendering context %i with Error Code %i",
			index, GetLastError() );
		return false;
	}

	return true;
}

bool CWindowsWindowAttached::UnBindRenderingContext(UINT index)
{
	if( !wglMakeCurrent( NULL, NULL ) )
	{
		error_fatal( "wglMakeCurrent: Failed to unbind the current rendering context" );
		return false;
	}

	return true;
}


int CWindowsWindowAttached::GetWidth()
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	return rect.right - rect.left;
}

int CWindowsWindowAttached::GetHeight()
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	return rect.bottom - rect.top;
}

bool CWindowsWindowAttached::GetCursorPos(int &x, int &y)
{
	return CWindowsWindow::GetCursorPos(m_hWnd, x, y);
}

bool CWindowsWindowAttached::SetCursorPos(int x, int y)
{
	return CWindowsWindow::SetCursorPos(m_hWnd, x, y);
}

bool CWindowsWindowAttached::SwapBuffers()
{
	return ::SwapBuffers(m_hDC) == TRUE;
}

bool CWindowsWindowAttached::IsFullscreen()
{
	return false;
}

bool CWindowsWindowAttached::IsActive()
{
	//return ::GetActiveWindow() == m_hWnd;
	return true;
}

void *CWindowsWindowAttached::GetWindowSystemReference()
{
	return (void*)m_hWnd;
}


#endif /* _WINDOWS */
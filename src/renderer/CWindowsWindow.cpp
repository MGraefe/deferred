// renderer/CWindowsWindow.cpp
// renderer/CWindowsWindow.cpp
// renderer/CWindowsWindow.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#ifdef _WINDOWS

#include "CWindowsWindow.h"
#include "CWindowCreateInfo.h"
#include "renderer.h"

CWindowsWindow::CWindowsWindow()
{
	m_hInstance = 0;
	m_hWnd = 0;
	m_hDC = 0;
	m_hRC[0] = NULL;
	m_hRC[1] = NULL;
	m_pixelFormat = 0;
	m_bFullscreen = false;
}


CWindowsWindow::~CWindowsWindow()
{

}


bool CWindowsWindow::Create(CWindowCreateInfo *info)
{
	WNDCLASS	wc;							// Windows Class Structure
	DWORD		dwExStyle;					// Window Extended Style
	DWORD		dwStyle;					// Window Style
	RECT		WindowRect;					// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;				// Set Left Value To 0
	WindowRect.right=(long)info->width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;					// Set Top Value To 0
	WindowRect.bottom=(long)info->height;	// Set Bottom Value To Requested Height

	//fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag
	m_bFullscreen = info->fullscreen;
	
	m_hInstance	= GetModuleHandle(NULL);						// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) info->eventCallback;		// eventCallback procedure handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= m_hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		error("Failed To Register The Window Class.");
		return false;											// Return FALSE
	}

	if (info->fullscreen)										// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= info->width;			// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= info->height;			// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= info->colorBits + info->alphaBits;	// Selected Bits Per Pixel
		dmScreenSettings.dmFields= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if(MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","Error", MB_YESNO|MB_ICONEXCLAMATION) == IDYES)
			{
				info->fullscreen = false;	// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				error("Error changing display settings. Program Will Now Close.");
				return false;									// Return FALSE
			}
		}
	}

	if(info->fullscreen) // Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;							// Window Extended Style
		dwStyle = WS_POPUP;										// Windows Style
		//ConsoleMessage("Hiding mouse pointer");
		//ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(m_hWnd = CreateWindowEx(	dwExStyle,	// Extended Style For The Window
		"OpenGL",							// Class Name
		info->title,						// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		m_hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		Kill();								// Reset The Display
		error("Window Creation Error.");
		return false;						// Return FALSE
	}

	//static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	//{
	//	sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
	//	1,											// Version Number
	//	PFD_DRAW_TO_WINDOW |						// Format Must Support Window
	//	PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
	//	PFD_DOUBLEBUFFER,							// Must Support Double Buffering
	//	PFD_TYPE_RGBA,								// Request An RGBA Format
	//	bits,										// Select Our Color Depth
	//	0, 0, 0, 0, 0, 0,							// Color Bits Ignored
	//	0,											// No Alpha Buffer
	//	0,											// Shift Bit Ignored
	//	0,											// No Accumulation Buffer
	//	0, 0, 0, 0,									// Accumulation Bits Ignored
	//	24,											// 24Bit Z-Buffer (Depth Buffer)  
	//	0,											// No Stencil Buffer
	//	0,											// No Auxiliary Buffer
	//	PFD_MAIN_PLANE,								// Main Drawing Layer
	//	0,											// Reserved
	//	0, 0, 0										// Layer Masks Ignored
	//};

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof(pfd) );
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = info->colorBits;
	pfd.cAlphaBits = info->alphaBits;
	pfd.cDepthBits = info->depthBits;
	pfd.iLayerType = PFD_MAIN_PLANE;

	m_hDC = GetDC(m_hWnd);
	if (!m_hDC)	// Did We Get A Device Context?
	{
		Kill(); // Reset The Display
		error("Can't Create A GL Device Context.");
		return false;
	}

	m_pixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	if (!m_pixelFormat) // Did Windows Find A Matching Pixel Format?
	{
		Kill();
		error("Can't Find A Suitable PixelFormat.");
		return false;
	}

	if(!SetPixelFormat(m_hDC, m_pixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		Kill();								// Reset The Display
		error("Can't Set The PixelFormat.");
		return false;								// Return FALSE
	}

	if (!(m_hRC[0] = wglCreateContext(m_hDC)))				// Are We Able To Get A Rendering Context?
	{
		Kill();								// Reset The Display
		error("Can't Create A GL Rendering Context.");
		return false;								// Return FALSE
	}

	if( !(m_hRC[1] = wglCreateContext(m_hDC)) )
	{
		Kill();
		error("Error creating the second OpenGL Rendering Context.");
		return false;
	}

	if( !wglShareLists(m_hRC[0], m_hRC[1]) )
	{
		DWORD errorCode = GetLastError();
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL);
		MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
		LocalFree(lpMsgBuf);

		//Destroy the GL context and just use 1 GL context
		wglDeleteContext(m_hRC[1]);
		m_hRC[1] = NULL;
	}

	ShowWindow(m_hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(m_hWnd);						// Slightly Higher Priority
	SetFocus(m_hWnd);									// Sets Keyboard Focus To The Window
	
	return TRUE;
}


void CWindowsWindow::Kill(void)
{
	if (m_bFullscreen)									// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}
	
	if (m_hRC[0])											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
			error("Release Of DC And RC Failed.");

		if (!wglDeleteContext(m_hRC[0]))						// Are We Able To Delete The RC?
			error("Release Rendering Context 1 Failed.");

		m_hRC[0] = NULL;										// Set RC To NULL
	}

	if(m_hRC[1])
	{
		if (!wglDeleteContext(m_hRC[1]))						// Are We Able To Delete The RC?
			error("Release Rendering Context 2 Failed.");
	}

	if (m_hDC && !ReleaseDC(m_hWnd, m_hDC))					// Are We Able To Release The DC
	{
		error("Release Device Context Failed.");
		m_hDC = NULL;										// Set DC To NULL
	}

	if (m_hWnd && !DestroyWindow(m_hWnd))					// Are We Able To Destroy The Window?
	{
		error("Could Not Release hWnd.");
		m_hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL", m_hInstance))			// Are We Able To Unregister Class
	{
		error("Could Not Unregister Class.");
		m_hInstance = NULL;									// Set hInstance To NULL
	}
}


void CWindowsWindow::HandleMessages(bool &quitMessage)
{
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
	{
		if(msg.message == WM_QUIT)				// Have We Received A Quit Message?
		{
			quitMessage = true;
			return;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


bool CWindowsWindow::HasSecondContext()
{
	return m_hRC[1] != NULL;
}


bool CWindowsWindow::BindRenderingContext(UINT index)
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



bool CWindowsWindow::UnBindRenderingContext(UINT index)
{
	if( !wglMakeCurrent( NULL, NULL ) )
	{
		error_fatal( "wglMakeCurrent: Failed to unbind the current rendering context" );
		return false;
	}

	return true;
}


bool CWindowsWindow::GetCursorPos(int &x, int &y)
{
	return GetCursorPos(m_hWnd, x, y);
}


bool CWindowsWindow::SetCursorPos(int x, int y)
{
	return SetCursorPos(m_hWnd, x, y);
}


bool CWindowsWindow::GetCursorPos(HWND hWnd, int &x, int &y)
{
	POINT p;
	if(::GetCursorPos(&p) == FALSE)
		return false;
	if(::ScreenToClient(hWnd, &p) == FALSE)
		return false;
	x = p.x;
	y = p.y;
	return true;
}


bool CWindowsWindow::SetCursorPos(HWND hWnd, int x, int y)
{
	POINT p;
	p.x = x;
	p.y = y;
	if(::ClientToScreen(hWnd, &p) == FALSE)
		return false;
	if(::SetCursorPos(p.x, p.y) == FALSE)
		return false;
	return true;
}


bool CWindowsWindow::SwapBuffers()
{
	return ::SwapBuffers(m_hDC) == TRUE;
}


bool CWindowsWindow::IsActive()
{
	if(IsFullscreen())
		return ::GetActiveWindow() == m_hWnd;
	else
		return ::IsIconic(m_hWnd) == FALSE;
}


bool CWindowsWindow::IsFullscreen()
{
	return m_bFullscreen;
}

void *CWindowsWindow::GetWindowSystemReference()
{
	return (void*)m_hWnd;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CWindowsWindow::GetWidth()
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	return rect.right - rect.left;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CWindowsWindow::GetHeight()
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	return rect.bottom - rect.top;
}

#endif /* _WINDOWS */

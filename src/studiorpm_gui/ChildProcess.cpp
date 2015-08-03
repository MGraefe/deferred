// studiorpm_gui/ChildProcess.cpp
// studiorpm_gui/ChildProcess.cpp
// studiorpm_gui/ChildProcess.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "StdAfx.h"
#include "ChildProcess.h"
#include "studiorpm_gui.h"
#include "studiorpm_guiDlg.h"

#define BUFSIZE 4096

ChildProcess::ChildProcess(void)
{
	m_childOutWr = NULL;
	m_pDlg = NULL;
}


ChildProcess::~ChildProcess(void)
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void ChildProcess::ReadOutputLoop( void )
{
	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE]; 
	BOOL bSuccess = FALSE;
	//HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	while( true )
	{ 
		DWORD exitCode;
		if( !GetExitCodeProcess( m_hProcess, &exitCode ) )
			break;

		bSuccess = ReadFile( m_childOutRd, chBuf, BUFSIZE, &dwRead, NULL);
		if( !bSuccess || dwRead == 0 ) 
			break;
		chBuf[dwRead] = '\0';

		//bSuccess = WriteFile(hParentStdOut, chBuf, dwRead, &dwWritten, NULL);
		CString str = CA2T( chBuf );
		m_pDlg->AddTextToLog( str );
		if ( !bSuccess ) 
			break; 

		if( exitCode != STILL_ACTIVE )
			break;

		m_pDlg->UpdateWindow();
	}

	CloseHandle( m_hProcess );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool ChildProcess::Start( CstudiorpmGuiDlg *dlg )
{
	m_pDlg = dlg;

	SECURITY_ATTRIBUTES saAttr; 

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 

	// Create a pipe for the child process's STDOUT. 
	if ( !CreatePipe(&m_childOutRd, &m_childOutWr, &saAttr, 0) ) 
	{
		dlg->AddTextToLog( _T("StdoutRd CreatePipe\n") ); 
		return false;
	}

	// Ensure the read handle to the pipe for STDOUT is not inherited.
	if ( !SetHandleInformation(m_childOutRd, HANDLE_FLAG_INHERIT, 0) )
	{
		dlg->AddTextToLog( _T("Stdout SetHandleInformation\n") );
		return false;
	}
	// Create a pipe for the child process's STDIN. 
	if (!CreatePipe(&m_childInRd, &m_childInWr, &saAttr, 0) ) 
	{
		dlg->AddTextToLog( _T("Stdin CreatePipe\n") );
		return false;
	}

	// Ensure the write handle to the pipe for STDIN is not inherited. 
	if ( !SetHandleInformation(m_childInWr, HANDLE_FLAG_INHERIT, 0) )
	{
		dlg->AddTextToLog( _T("Stdin SetHandleInformation\n") ); 
		return false;
	}


	//Create child process
	//TCHAR szAppName[] = _T( "studiorpm.exe" );
	CString cmdLine = CreateCommandLine();
	TCHAR *szCmdLine = cmdLine.GetBuffer();
	m_pDlg->AddTextToLog( CString(_T("Command: ")) + CString(szCmdLine) );
	

	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo;
	BOOL bSuccess = FALSE; 

	// Set up members of the PROCESS_INFORMATION structure. 
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.hStdError = m_childOutWr;
	siStartInfo.hStdOutput = m_childOutWr;
	siStartInfo.hStdInput = m_childInRd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process. 
	bSuccess = CreateProcess(
		NULL,	   // application name
		szCmdLine,     // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION 

	// If an error occurs, exit the application. 
	if ( !bSuccess ) 
	{
		dlg->AddTextToLog( _T("CreateProcess") );
		return false;
	}
	else 
	{
		// Close handles to the child process and its primary thread.
		// Some applications might keep these handles to monitor the status
		// of the child process, for example. 
		m_hProcess = piProcInfo.hProcess;
		//CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
		return true;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CString ChildProcess::CreateCommandLine( void )
{
	CString appName = _T("studiorpm.exe");

	CString inputFile;
	m_pDlg->m_EditFileInput.GetWindowText(inputFile);

	CString outputFile;
	m_pDlg->m_EditFileOutput.GetWindowText(outputFile);

	CString texPrefix;
	m_pDlg->m_EditTexturePath.GetWindowText(texPrefix);

	CString scale;
	m_pDlg->m_EditScale.GetWindowText(scale);

	CString rotate;
	m_pDlg->m_EditRotate.GetWindowText(rotate);
	
	CString cmdLine = appName + 
		" \"" + inputFile + "\" \"" + outputFile + 
		"\" \"" + texPrefix + "\" " + scale + " " + rotate;

	return cmdLine;
}

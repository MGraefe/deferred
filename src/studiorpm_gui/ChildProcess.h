// studiorpm_gui/ChildProcess.h
// studiorpm_gui/ChildProcess.h
// studiorpm_gui/ChildProcess.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

//ChildProcess.h

#pragma once
#ifndef deferred__studiorpm_gui__ChildProcess_H__
#define deferred__studiorpm_gui__ChildProcess_H__

class CstudiorpmGuiDlg;
class ChildProcess
{
public:
	ChildProcess(void);
	~ChildProcess(void);

	bool Start( CstudiorpmGuiDlg *dlg );
	void ReadOutputLoop( void );

private:
	CString CreateCommandLine( void );
	HANDLE m_childOutWr;
	HANDLE m_childOutRd;
	HANDLE m_childInWr;
	HANDLE m_childInRd;
	HANDLE m_hProcess;
	CstudiorpmGuiDlg *m_pDlg;
};


#endif // deferred__studiorpm_gui__ChildProcess_H__


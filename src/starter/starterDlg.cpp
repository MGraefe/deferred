// starter/starterDlg.cpp
// starter/starterDlg.cpp
// starter/starterDlg.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


// starterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "starter.h"
#include "starterDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include <sstream>
#include <filesystem/include.h>
#include <util/instream.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CstarterDlg dialog




CstarterDlg::CstarterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CstarterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CstarterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbResolution);
	DDX_Control(pDX, IDC_EDIT1, m_eIP);
	DDX_Control(pDX, IDC_CHECK1, m_bFullscreen);
	DDX_Radio(pDX, IDC_RADIO1, m_iConnectChoice);
	DDX_Control(pDX, IDC_COMBO2, m_cbMap);
	DDX_Control(pDX, IDC_EDIT2, m_eNickname);
}

BEGIN_MESSAGE_MAP(CstarterDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDOK, &CstarterDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CstarterDlg message handlers
struct regvals_t
{
	CString resolution;
	int fullscreenChoice;
	int connectChoice;
	CString ip;
	CString map;
	CString nickname;
};


const TCHAR regResolution[] = _T("resolution");
const TCHAR regFullscreen[] = _T("fullscreen");
const TCHAR regConnectChoice[] = _T("connectChoice");
const TCHAR regIp[] = _T("ip");
const TCHAR regMap[] = _T("map");
const TCHAR regNickname[] = _T("nickname");

void ReadRegistry(regvals_t &v)
{
	TCHAR s[] = _T("");
	v.resolution = theApp.GetProfileString(s, regResolution, _T("1366x768"));
	v.fullscreenChoice = theApp.GetProfileInt(s, regFullscreen, 1);
	v.connectChoice = theApp.GetProfileInt(s, regConnectChoice, 0);
	v.ip = theApp.GetProfileString(s, regIp, _T("192.168.0.42"));
	v.map = theApp.GetProfileString(s, regMap, _T(""));
	v.nickname = theApp.GetProfileString(s, regNickname, _T("Player"));
}

void WriteRegistry(const regvals_t &v)
{
	TCHAR s[] = _T("");
	theApp.WriteProfileString(s, regResolution, v.resolution);
	theApp.WriteProfileInt(s, regFullscreen, v.fullscreenChoice);
	theApp.WriteProfileInt(s, regConnectChoice, v.connectChoice);
	theApp.WriteProfileString(s, regIp, v.ip);
	theApp.WriteProfileString(s, regMap, v.map);
	theApp.WriteProfileString(s, regNickname, v.nickname);
}


BOOL CstarterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	regvals_t regVals;
	ReadRegistry(regVals);

	InitResolutions();

	InitMaps();

	int resId = m_cbResolution.FindStringExact(-1, regVals.resolution);
	if(resId == CB_ERR)
		m_cbResolution.AddString(regVals.resolution);
	m_cbResolution.SelectString(-1, regVals.resolution);

	m_bFullscreen.SetCheck(regVals.fullscreenChoice);
	m_iConnectChoice = regVals.connectChoice;
	m_eIP.SetWindowText(regVals.ip);

	std::set<TCHAR> charset;
	for(TCHAR c = _T('a'); c <= _T('z'); c++)
		charset.insert(c);
	for(TCHAR c = _T('A'); c <= _T('Z'); c++)
		charset.insert(c);
	for(TCHAR c = _T('0'); c <= _T('9'); c++)
		charset.insert(c);
	charset.insert('_');

	m_eNickname.SetMask(charset);
	m_eNickname.SetWindowText(regVals.nickname);

	if(!regVals.map.IsEmpty())
		m_cbMap.SetWindowText(regVals.map);
	else
		m_cbMap.SetCurSel(0);

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


bool IsValidResolution( const char *pStr )
{
	int x = 0;
	int y = 0;

	int vals = sscanf(pStr, "%ix%i", &x, &y);
	return vals == 2 && x > 10 && x < 20000 && y > 10 && y < 20000;	
}

#define ARRAY_SIZE(arr) sizeof(arr)/sizeof(arr[0])
const char* defaultResolutions[] = { "800x600", "1024x768", "1280x1024", "1366x768", "1600x1200", "1920x1080", "1920x1200" };

void CstarterDlg::InitResolutions( void )
{
	fs::ifstream is( "scripts\\resolutions.txt" );
	if( !is.is_open() )
	{
		is.close();

		for( int i = 0; i < ARRAY_SIZE(defaultResolutions); i++ )
			m_cbResolution.AddString( defaultResolutions[i] );

		MessageBox( "'scripts\\resolutions.txt' not found, created default resolutions", "Error" );
		return;
	}

	char str[16];
	while( is.getline(str, 16) )
	{
		if( !IsValidResolution(str) )
			continue;
		m_cbResolution.AddString(str);
	}

	is.close();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CstarterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CstarterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CstarterDlg::OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	*pResult = 0;
}


int CstarterDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;



	return 0;
}


void CstarterDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	char resx[64];
	m_cbResolution.GetWindowText(resx, 64);
	if( !IsValidResolution( resx ) )
	{
		MessageBox( "Please enter a valid resolution.", "Error" );
		return;
	}

	char *x = strchr( resx, 'x' );
	*x = 0;
	char *resy = x+1;

	int windowed = m_bFullscreen.GetCheck() == 0 ? 1 : 0;
	
	char mapname[64];
	m_cbMap.GetWindowText(mapname, 64);

	char nickname[32];
	m_eNickname.GetWindowText(nickname, 32);

	std::ostringstream params;
	params << "-resx=" << resx << " -resy=" << resy << " -windowed=" << windowed << " -map=" << mapname << " -name=\"" << nickname << "\"";
	
	char chIP[64] = "";
	if( m_iConnectChoice == 1 )
	{
		m_eIP.GetWindowTextA( chIP, 63 );
		params << " -ip=" << chIP;
	}

	SaveToReg();

	ShellExecute( NULL, "open", "deferred.exe", params.str().c_str(), NULL, SW_SHOWNORMAL );

	CDialogEx::OnOK();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CstarterDlg::InitMaps()
{
	filesystem::Filesystem &fs = filesystem::Filesystem::GetInstance();
	
	filesystem::findFileInfo_t info;
	HANDLE handle = fs.findFirstFile("models", "*.dfmap", &info);
	if(!handle)
		return;
	do
	{
		if(info.isRegularFile && !info.isFolder && m_cbMap.FindStringExact(0, info.name) == CB_ERR)
			m_cbMap.AddString(std::string(info.name, strlen(info.name)-6).c_str());
	} while(fs.findNextFile(handle, &info));

	fs.closeFindFile(handle);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CstarterDlg::SaveToReg()
{
	regvals_t regVals;
	m_cbResolution.GetWindowText(regVals.resolution);
	regVals.fullscreenChoice = m_bFullscreen.GetCheck();
	regVals.connectChoice = m_iConnectChoice;
	m_eIP.GetWindowText(regVals.ip);
	m_cbMap.GetWindowText(regVals.map);
	m_eNickname.GetWindowText(regVals.nickname);
	WriteRegistry(regVals);
}

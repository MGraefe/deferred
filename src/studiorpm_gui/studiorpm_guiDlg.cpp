// studiorpm_gui/studiorpm_guiDlg.cpp
// studiorpm_gui/studiorpm_guiDlg.cpp
// studiorpm_gui/studiorpm_guiDlg.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

// studiorpm_guiDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "studiorpm_gui.h"
#include "studiorpm_guiDlg.h"
#include "afxdialogex.h"
#include "ChildProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialogfelddaten
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CstudiorpmGuiDlg-Dialogfeld




CstudiorpmGuiDlg::CstudiorpmGuiDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CstudiorpmGuiDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_childProcess = NULL;
}

void CstudiorpmGuiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILE_INPUT, m_EditFileInput);
	DDX_Control(pDX, IDC_EDIT_FILE_OUTPUT, m_EditFileOutput);
	DDX_Control(pDX, IDC_EDIT_TEXTURE_PREFIX, m_EditTexturePath);
	DDX_Control(pDX, IDC_EDIT_SCALE, m_EditScale);
	DDX_Control(pDX, IDC_EDIT_CONSOLE_OUT, m_EditConsole);
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressBar);
	DDX_Control(pDX, IDC_EDIT1, m_EditRotate);
}

BEGIN_MESSAGE_MAP(CstudiorpmGuiDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_BROWSE_INPUT, &CstudiorpmGuiDlg::OnBnClickedBtnBrowseInput)
	ON_BN_CLICKED(IDC_BTN_BROWSE_OUTPUT, &CstudiorpmGuiDlg::OnBnClickedBtnBrowseOutput)
	ON_BN_CLICKED(IDC_BTN_SCALE_DEFAULT, &CstudiorpmGuiDlg::OnBnClickedBtnScaleDefault)
	ON_BN_CLICKED(IDC_BTN_SCALE_INCH_DM, &CstudiorpmGuiDlg::OnBnClickedBtnScaleInchDm)
	ON_BN_CLICKED(ID_START, &CstudiorpmGuiDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_BTN_OUTPUT_DEFAULT, &CstudiorpmGuiDlg::OnBnClickedBtnOutputDefault)
	ON_BN_CLICKED(IDC_BTN_ROTATE_DEFAULT, &CstudiorpmGuiDlg::OnBnClickedBtnRotateDefault)
END_MESSAGE_MAP()


// CstudiorpmGuiDlg-Meldungshandler

BOOL CstudiorpmGuiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	m_EditTexturePath.SetWindowText( _T("models/") );
	m_EditScale.SetWindowText( _T("1.0") );
	m_EditRotate.SetWindowText( _T("0 0 0") );
	TCHAR windowText[] =	_T("StudioRPM Gui, Build: ") _T(__DATE__) _T(" ") _T(__TIME__) \
							_T("\nCopyright (c) 2012 Marius Gräfe\n\n");
	AddTextToLog( windowText );

	return TRUE;  // TRUE zurückgeben, wenn der Fokus nicht auf ein Steuerelement gesetzt wird
}

void CstudiorpmGuiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CstudiorpmGuiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CstudiorpmGuiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CstudiorpmGuiDlg::OnBnClickedBtnBrowseInput()
{
	TCHAR defaultPath[2048];
	if( m_EditFileInput.GetWindowTextLength() == 0 )
	{
		GetCurrentDirectory( 2048-1, defaultPath );
		_tcscat_s( defaultPath, _T("\\file.smd") );
	}
	else
		m_EditFileInput.GetWindowText( defaultPath, 2048-1 );
	
	TCHAR strFilter[] = _T("SMD Files (*.smd)|*.smd|All Files (*.*)|*.*||");

	CFileDialog inDlg( TRUE, _T(".smd"), defaultPath, 6UL, strFilter, this );
	if( inDlg.DoModal() != IDOK )
		return;

	m_EditFileInput.SetWindowText( inDlg.GetPathName() );

	if( m_EditFileOutput.GetWindowTextLength() == 0 )
		OnBnClickedBtnOutputDefault();
}


void CstudiorpmGuiDlg::OnBnClickedBtnBrowseOutput()
{
	TCHAR defaultPath[2048];
	if( m_EditFileInput.GetWindowTextLength() == 0 )
		GetCurrentDirectory( 2048-1, defaultPath );
	else
		m_EditFileOutput.GetWindowText( defaultPath, 2048-1 );

	TCHAR strFilter[] = _T("RPM Files (*.rpm)|*.rpm|All Files (*.*)|*.*||");

	CFileDialog inDlg( FALSE, _T(".smd"), defaultPath, 6UL, strFilter, this );
	if( inDlg.DoModal() != IDOK )
		return;

	m_EditFileOutput.SetWindowText( inDlg.GetPathName() );
}


void CstudiorpmGuiDlg::OnBnClickedBtnScaleDefault()
{
	m_EditScale.SetWindowText( _T("1.0") );
}


void CstudiorpmGuiDlg::OnBnClickedBtnScaleInchDm()
{
	m_EditScale.SetWindowText( _T("0.254") );
}


void CstudiorpmGuiDlg::OnBnClickedStart()
{
	m_ProgressBar.SetMarquee( TRUE, 20 );
	SpawnChildProcess();
	m_ProgressBar.SetMarquee( FALSE, 20 );
}


void CstudiorpmGuiDlg::OnBnClickedBtnOutputDefault()
{
	CString wndText;
	m_EditFileInput.GetWindowText( wndText );

	if( wndText.GetLength() == 0 )
		return;

	int dotPos = wndText.ReverseFind( '.' );
	if( dotPos == -1 )
		return;

	wndText.Truncate( dotPos+1 );
	wndText.Append( _T("rpm") );

	m_EditFileOutput.SetWindowText( wndText );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CstudiorpmGuiDlg::AddTextToLog( CString str )
{
	str.Replace( _T("\n"), _T("\r\n") );
	m_EditConsole.SetSel( m_EditConsole.GetWindowTextLength(), m_EditConsole.GetWindowTextLength() );
	m_EditConsole.ReplaceSel( str );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CstudiorpmGuiDlg::SpawnChildProcess( void )
{
	m_childProcess = new ChildProcess();
	if( m_childProcess->Start(this) )
		m_childProcess->ReadOutputLoop();
	delete m_childProcess;
}


void CstudiorpmGuiDlg::OnBnClickedBtnRotateDefault()
{
	m_EditRotate.SetWindowText( _T("0 0 0") );
}

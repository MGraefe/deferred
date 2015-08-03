// editor/editor.cpp
// editor/editor.cpp
// editor/editor.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
operator=

// editor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "editor.h"
#include "MainFrm.h"
#include "buildinfo.h"

#include "ChildFrm.h"
#include "editorDoc.h"
#include "editorView.h"
#include "CEntityDialog.h"
#include "COpenGLView.h"
#include "CDlgNewEntity.h"
#include "CEditorEntityList.h"
#include "C3dView.h"
#include "CEditorEntity.h"
#include <util/ConsoleHandler.h>
#include <util/CConVar.h>
#include <consoleCallback.h>
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CeditorApp

BEGIN_MESSAGE_MAP(CeditorApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CeditorApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_EDIT_NEWENTITY, &CeditorApp::OnEditNewentity)
	ON_UPDATE_COMMAND_UI(ID_EDIT_NEWENTITY, &CeditorApp::OnUpdateEditNewentity)
	ON_COMMAND(ID_VIEW_TOGGLELIGHTING, &CeditorApp::OnViewTogglelighting)
	ON_COMMAND(ID_VIEW_TOGGLEWATER, &CeditorApp::OnViewTogglewater)
	ON_COMMAND(ID_EDIT_DUPLICATESELECTED, &CeditorApp::OnEditDuplicateselected)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DUPLICATESELECTED, &CeditorApp::OnUpdateEditDuplicateselected)
END_MESSAGE_MAP()


// CeditorApp construction

CeditorApp::CeditorApp()
{
	m_entityClipboardFormat = 0;
	m_bHiColorIcons = TRUE;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("redpuma.deferred.defedt.v1"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CeditorApp object

CeditorApp theApp;


// CeditorApp initialization

BOOL CeditorApp::InitInstance()
{
	InitConsole();
	m_pConsoleHandler = new ConsoleHandler();
	m_pConsoleHandler->AddCallback( &CConVarList::HandleConsoleInput, &CConVarList::GetConVarBase );
	InitConsoleInputCallbackRenderer( m_pConsoleHandler );
	m_pConsoleHandler->Start();

	//Init filesystem
	filesystem::Filesystem &filesys = filesystem::Filesystem::GetInstance();
	filesys.mountArchive("data.rpaf");

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("redpuma.deferred"));
	LoadStdProfileSettings(6);  // Load standard INI file options (including MRU)

	m_entityClipboardFormat = RegisterClipboardFormat(_T("deferred.editor.clipboard.entities"));
	if(m_entityClipboardFormat == 0)
		error_fatal("Registering the clipboard format failed.");

	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();

	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_UntitledTYPE,
		RUNTIME_CLASS(CeditorDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(COpenGLView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//Dont open up an initial window
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CeditorApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CeditorApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnShowWindow(BOOL bShow, UINT nStatus);
// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_abouttext;
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ABOUT_TEXT, m_abouttext);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
BOOL CAboutDlg::OnInitDialog()
{
	if( CDialogEx::OnInitDialog() == FALSE )
		return FALSE;

	std::string wndText = std::string("defedt for deferred ") + GetDeferredVersion() + " - build: " + g_date_string + ", " + g_time_string;
#ifdef _DEBUG
	wndText += " - DEBUG BUILD";
#endif
	m_abouttext.SetWindowText(CString(wndText.c_str()));

	return TRUE;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CAboutDlg::OnShowWindow( BOOL bShow, UINT nStatus )
{	
	CDialogEx::OnShowWindow(bShow, nStatus);
}


// App command to run the dialog
void CeditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CeditorApp customization load/save methods

void CeditorApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CeditorApp::LoadCustomState()
{
}

void CeditorApp::SaveCustomState()
{
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorApp::OnOpenEntityProperties()
{

}


// CeditorApp message handlers




//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorApp::OnEditNewentity()
{
	CeditorDoc *pDoc = GetActiveDocument();
	if( !pDoc )
		return;

	CDlgNewEntity dlg( (CWnd*)(((CMainFrame*)AfxGetMainWnd())->MDIGetActive()) );
	if( dlg.DoModal() == IDOK )
	{
		CEditorEntity *pEnt = pDoc->GetEntityList()->InsertNewEntity( dlg.m_entityType );
		pDoc->SelectSingleEntity(pEnt);
		CCamera *pCam = ((C3dView*)GetActiveChildFrame()->GetView(VIEW_3D)->GetRenderView())->GetCamera();
		pEnt->SetAbsCenter( pCam->GetAbsPos() + pCam->GetAbsAngles()*Vector3f(0.0f,0.0f,-1.0f)*10.0f );
		//pDoc->UpdateAllViews(NULL);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorApp::OnUpdateEditNewentity(CCmdUI *pCmdUI)
{
	CeditorDoc *pDoc = GetActiveDocument();
	pCmdUI->Enable(pDoc != NULL);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorApp::ToggleConVar(const std::string &name)
{
	CeditorDoc *pDoc = GetActiveDocument();
	if( pDoc )
	{
		CConVarBase *pCVarBase = GetConsoleHandler()->GetConVarBase(name);
		if( pCVarBase && pCVarBase->IsConVar() )
		{
			CConVar *pCVar = (CConVar*)pCVarBase;
			pCVar->SetValue(!pCVar->GetBool());
			//pDoc->UpdateAllViews(NULL);
		}
		else
			error("Could not find cvar %s", name.c_str());
	}
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorApp::OnViewTogglelighting()
{
	ToggleConVar("render.unlit");
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CeditorApp::OnViewTogglewater()
{
	ToggleConVar("render.water.draw");
}


void CeditorApp::OnEditDuplicateselected()
{
	CeditorDoc *pDoc = GetActiveDocument();
	if(pDoc)
		pDoc->DuplicateSelectedEntities();
}


void CeditorApp::OnUpdateEditDuplicateselected(CCmdUI *pCmdUI)
{
	CeditorDoc *pDoc = GetActiveDocument();
	pCmdUI->Enable(pDoc && !pDoc->GetSelectedEntityList().empty());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CLIPFORMAT CeditorApp::GetEntityClipboardFormat() const
{
	return m_entityClipboardFormat;
}

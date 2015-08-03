// editor/CEntityDialogClassInfo.cpp
// editor/CEntityDialogClassInfo.cpp
// editor/CEntityDialogClassInfo.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

// CEntityDialogClassInfo.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "CEntityDialogClassInfo.h"
#include "afxdialogex.h"
#include "CEntityRegister.h"
#include "CDlgEntityValColor.h"
#include "editorDoc.h"
#include "CEditorEntity.h"
#include <util/StringUtils.h>

// CEntityDialogClassInfo-Dialogfeld

IMPLEMENT_DYNAMIC(CEntityDialogClassInfo, CDialogEx)

CEntityDialogClassInfo::CEntityDialogClassInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEntityDialogClassInfo::IDD, pParent)
{
	m_activeDialog = NULL;
	m_editEnt = NULL;
	m_bClosing = false;
}

CEntityDialogClassInfo::~CEntityDialogClassInfo()
{
	for( auto it = m_valueTypesMap.begin(); it != m_valueTypesMap.end(); it++ )
	{
		delete it->second;
	}
}


BOOL CEntityDialogClassInfo::OnInitDialog( void )
{
	if( !CDialogEx::OnInitDialog() )
		return FALSE;

	if( !m_editEnt )
		return FALSE;

	//Fill the class list combob
	std::vector<std::string> entNames;
	GetEntityRegister()->GetEntityNames(entNames);
	
	for( UINT i = 0; i < entNames.size(); i++ )
	{
		if( entNames[i][0] != '@' )
		{
			m_cbClass.AddString( CA2T(entNames[i].c_str()) );
		}
	}

	int index = m_cbClass.FindString(-1, m_editEnt->GetClassnameW().c_str() );
	if( index >= 0 )
		m_cbClass.SetCurSel(index);

	CreateValueTypeDialogs();

	OnCbnSelchangeCbClass();
	
	return TRUE;
}


void CEntityDialogClassInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_CLASS, m_cbClass);
	DDX_Control(pDX, IDC_LIST_KEY, m_lbKey);
	DDX_Control(pDX, IDC_GB_VALUE, m_gbValue);
	DDX_Control(pDX, IDC_EDIT_HELP, m_ecHelp);
	DDX_Control(pDX, IDC_EDIT_VALUE_RAW_TEXT, m_ecValueRaw);
}


BEGIN_MESSAGE_MAP(CEntityDialogClassInfo, CDialogEx)
	ON_CBN_SELCHANGE(IDC_CB_CLASS, &CEntityDialogClassInfo::OnCbnSelchangeCbClass)
	ON_LBN_SELCHANGE(IDC_LIST_KEY, &CEntityDialogClassInfo::OnLbnSelchangeListKey)
	ON_BN_CLICKED(IDC_BTN_SELECT_COLOR, &CEntityDialogClassInfo::OnBnClickedSelectColor)
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE_RAW_TEXT, &CEntityDialogClassInfo::OnEnKillfocusEditValueRawText)
END_MESSAGE_MAP()




//Called when selection inside the combobox has changed
void CEntityDialogClassInfo::OnCbnSelchangeCbClass()
{
	m_lbKey.ResetContent();

	//Get current selected class name
	std::string classname;
	CString selection;
	int sel = m_cbClass.GetCurSel();
	if( sel == CB_ERR )
		return;
	m_cbClass.GetLBText( m_cbClass.GetCurSel(), selection );
	classname = CT2A(selection);


	CEntityRegisterEntity *pEnt = GetEntityRegister()->GetEntity(classname);
	if( !pEnt )
		return;

	for( int i = 0; i < pEnt->size(); i++ )
	{
		m_lbKey.AddString( CA2T( pEnt->Get(i)->name.c_str() ) );
	}

	//select first index in key list
	m_lbKey.SetCurSel(0);
	OnLbnSelchangeListKey();
}


//Called when selection in the key-field changes
void CEntityDialogClassInfo::OnLbnSelchangeListKey()
{
	int sel = m_lbKey.GetCurSel();
	if( sel == LB_ERR )
		return;

	//Get the selected key
	CString selKey;
	m_lbKey.GetText( sel, selKey );

	//Get the selected class
	CString selClass;
	m_cbClass.GetLBText( m_cbClass.GetCurSel(), selClass );

	//Get the entity-stub from the entity register
	CEntityRegisterEntity *pEnt = GetEntityRegister()->GetEntity(selClass);
	if( !pEnt )
		return;

	//Get the value-stub
	m_currentKey = pEnt->Get( selKey );
	if( !m_currentKey )
		return;

	//Set help text
	m_ecHelp.SetWindowText( CA2T(m_currentKey->desc.c_str()) );

	//is an value-edit dialog open? the close!
	if( m_activeDialog )
	{
		m_activeDialog->ShowWindow(SW_HIDE);
		RedrawWindow();
	}

	//Get value-edit dialog for specified value type
	auto it = m_valueTypesMap.find(m_currentKey->type);
	if( it != m_valueTypesMap.end() )
	{
		m_activeDialog = it->second;
		m_activeDialog->ShowWindow(SW_SHOW);
	}
	else
		m_activeDialog = NULL;

	//Get the real value of the entity from the entity.
	std::string value = m_editEnt->GetValue( m_currentKey->name );
	m_ecValueRaw.SetWindowText( CA2T(value.c_str()) );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
#define CREATE_AND_ADD_DIALOG(type, classname, templ) \
	m_valueTypesMap[type] = new classname(); \
	m_valueTypesMap[type]->Create( templ, this ); \
	m_valueTypesMap[type]->SetWindowPos( NULL, rect.left, rect.top, rect.Width(), rect.Height(), NULL )

void CEntityDialogClassInfo::CreateValueTypeDialogs( void )
{
	CRect rect;
	m_gbValue.GetWindowRect( &rect );
	CRect parentRect;
	GetWindowRect(parentRect);
	rect.OffsetRect( -parentRect.TopLeft() );
	rect.DeflateRect(5,40,5,5);

	CREATE_AND_ADD_DIALOG(PT_COLOR, CDlgEntityValColor, IDD_ENTITY_VAL_COLOR_BRIGHTNESS);
}


std::map<HWND, CEntityDialogClassInfo*> hookMap;
UINT_PTR CALLBACK CCHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if(uiMsg == WM_INITDIALOG)
	{
		CHOOSECOLOR *cc = (CHOOSECOLOR*)lParam;
		hookMap[hdlg] = (CEntityDialogClassInfo*)cc->lCustData;
	}
	else if(uiMsg == WM_DESTROY)
	{
		hookMap.erase(hdlg);
	}
	else if(uiMsg == WM_CTLCOLORSTATIC)
	{
		TCHAR szR[256];
		szR[0] = 0;
		::GetDlgItemText(hdlg, 0x2c2, szR, 256);
		TCHAR szG[256];
		szG[0] = 0;
		::GetDlgItemText(hdlg, 0x2c3, szG, 256);
		TCHAR szB[256];
		szB[0] = 0;
		::GetDlgItemText(hdlg, 0x2c4, szB, 256);

		COLORREF color = RGB(_ttoi(szR), _ttoi(szG), _ttoi(szB));
		
		if(hookMap.find(hdlg) != hookMap.end())
			hookMap[hdlg]->UpdateColor(color);
	}

	return 0;
}

COLORREF g_colorRefs[16] = {0};
//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityDialogClassInfo::OnBnClickedSelectColor()
{
	CString currentColor;
	m_ecValueRaw.GetWindowText(currentColor);
	Vector3f vCurrentColor = StrUtils::GetVector3(CT2A(currentColor)) + 0.5f;
	COLORREF colorref = RGB(vCurrentColor.x, vCurrentColor.y, vCurrentColor.z);

	CHOOSECOLOR cc;
	memset(&cc, 0, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = this->GetSafeHwnd();
	cc.lCustData = (LPARAM)(this);
	cc.lpCustColors = g_colorRefs;
	cc.lpfnHook = &CCHookProc;
	cc.rgbResult = colorref;
	cc.Flags = CC_ANYCOLOR | CC_ENABLEHOOK | CC_FULLOPEN | CC_RGBINIT;
	if(::ChooseColor(&cc))
		UpdateColor(cc.rgbResult);
	else
		UpdateColor(colorref);
}


void CEntityDialogClassInfo::UpdateColor(COLORREF color)
{
	int r = GetRValue(color);
	int g = GetGValue(color);
	int b = GetBValue(color);

	CString cl;
	cl.Format( _T("%i %i %i"), r, g, b );

	CString brightness;
	m_ecValueRaw.GetWindowText(brightness);
	brightness.Delete( 0, brightness.ReverseFind(' ') );
	
	m_ecValueRaw.SetWindowText( cl + brightness );
	UpdateCurrentParam();
}


void CEntityDialogClassInfo::OnEnKillfocusEditValueRawText()
{
	UpdateCurrentParam();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityDialogClassInfo::UpdateCurrentParam( void )
{
	if( !m_editEnt )
		return;

	CString csNewValue;
	m_ecValueRaw.GetWindowText( csNewValue );

	std::string newValue( CT2A(csNewValue).m_psz );

	m_editEnt->SetValue( m_currentKey->name, newValue );

	//GetActiveDocument()->UpdateAllViews(NULL);
}


void CEntityDialogClassInfo::OnOK()
{
	UpdateCurrentParam();
	//CDialogEx::OnOK();
}


void CEntityDialogClassInfo::OnCancel()
{
	//Do absoluteley nothing here.

	//CDialogEx::OnCancel();
}

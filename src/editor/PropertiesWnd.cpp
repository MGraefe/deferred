// editor/PropertiesWnd.cpp
// editor/PropertiesWnd.cpp
// editor/PropertiesWnd.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "editor.h"
#include "CustColorProperty.h"
#include "editorDoc.h"
#include "CEditorEntity.h"
#include "CEditorEntityList.h"
#include "CEntityRegister.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROP_INOUTPUTS, OnPropertiesInOut)
	ON_UPDATE_COMMAND_UI(ID_PROP_INOUTPUTS, OnUpdatePropertiesInOut)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	m_wndObjectCombo.GetWindowRect(&rectCombo);

	int cyCmb = rectCombo.Size().cy;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create Properties Combo \n");
		return -1;      // fail to create
	}

	m_wndObjectCombo.AddString(_T("Application"));
	m_wndObjectCombo.AddString(_T("Properties Window"));
	m_wndObjectCombo.SetCurSel(0);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}



void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}


void CPropertiesWnd::OnPropertiesInOut()
{
	CMainFrame *mainFrame = (CMainFrame*)AfxGetMainWnd();
	CBasePane *pane = mainFrame->GetPane(ID_INOUT_WND_PANE);
	if(!pane->IsVisible())
		pane->ShowPane(TRUE, FALSE, TRUE);
	else
		pane->ShowPane(FALSE, FALSE, FALSE);
}

void CPropertiesWnd::OnUpdatePropertiesInOut(CCmdUI* pCmdUI)
{
	CMainFrame *mainFrame = (CMainFrame*)AfxGetMainWnd();
	CBasePane *pane = mainFrame->GetPane(ID_INOUT_WND_PANE);
	pCmdUI->SetCheck(pane->IsVisible());
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	//CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Appearance"));

	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), (_variant_t) false, _T("Specifies the window's font will be non-bold and controls will have a 3D border")));

	//CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Border"), _T("Dialog Frame"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
	//pProp->AddOption(_T("None"));
	//pProp->AddOption(_T("Thin"));
	//pProp->AddOption(_T("Resizable"));
	//pProp->AddOption(_T("Dialog Frame"));
	//pProp->AllowEdit(FALSE);

	//pGroup1->AddSubItem(pProp);
	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (_variant_t) _T("About"), _T("Specifies the text that will be displayed in the window's title bar")));

	//m_wndPropList.AddProperty(pGroup1);

	//CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Window Size"), 0, TRUE);

	//pProp = new CMFCPropertyGridProperty(_T("Height"), (_variant_t) 250l, _T("Specifies the window's height"));
	//pProp->EnableSpinControl(TRUE, 50, 300);
	//pSize->AddSubItem(pProp);

	//pProp = new CMFCPropertyGridProperty( _T("Width"), (_variant_t) 150l, _T("Specifies the window's width"));
	//pProp->EnableSpinControl(TRUE, 50, 200);
	//pSize->AddSubItem(pProp);

	//m_wndPropList.AddProperty(pSize);

	//CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Font"));

	//LOGFONT lf;
	//CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	//font->GetLogFont(&lf);

	//lstrcpy(lf.lfFaceName, _T("Arial"));

	//pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("Font"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("Specifies the default font for the window")));
	//pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Use System Font"), (_variant_t) true, _T("Specifies that the window uses MS Shell Dlg font")));

	//m_wndPropList.AddProperty(pGroup2);

	//CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Misc"));
	//pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Application"));
	//pProp->Enable(FALSE);
	//pGroup3->AddSubItem(pProp);

	//CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Old Color Picker"), RGB(210, 192, 254), NULL, _T("Specifies the default window color"));
	//pColorProp->EnableOtherButton(_T("Other..."));
	//pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	//pGroup3->AddSubItem(pColorProp);

	//CCustColorProperty *pColorProp2 = new CCustColorProperty(_T("New Color Picker"), RGB(210, 192, 254), 300, NULL, _T("Specifies the default window color"));
	//pColorProp2->EnableOtherButton(_T("Other..."));
	//pColorProp2->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	//pGroup3->AddSubItem(pColorProp2);

	//static const TCHAR szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon")));

	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));

	//m_wndPropList.AddProperty(pGroup3);

	//CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("Hierarchy"));

	//CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("First sub-level"));
	//pGroup4->AddSubItem(pGroup41);

	//CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("Second sub-level"));
	//pGroup41->AddSubItem(pGroup411);

	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 1"), (_variant_t) _T("Value 1"), _T("This is a description")));
	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 2"), (_variant_t) _T("Value 2"), _T("This is a description")));
	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 3"), (_variant_t) _T("Value 3"), _T("This is a description")));

	//pGroup4->Expand(FALSE);
	//m_wndPropList.AddProperty(pGroup4);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}


void CPropertiesWnd::UpdateEntitySelection(CeditorDoc *pActiveDoc)
{
	//rebuild properties from scratch
	m_wndPropList.RemoveAll();

	CeditorDoc *pDoc = pActiveDoc;
	if( !pDoc )
		pDoc = GetActiveDocument();
	if(	!pDoc )
		return;

	const SelectedEntityList &selEnts = pDoc->GetSelectedEntityList();
	if( selEnts.empty() )
		return;

	CMFCPropertyGridProperty *commonKVsGroup = new CMFCPropertyGridProperty(_T("Common keyvals"), 1);
	m_wndPropList.AddProperty(commonKVsGroup);
	commonKVsGroup->SetDescription(_T("Common keyvals among different classes (multi-selection)"));

	std::map<string, CMFCPropertyGridProperty*, LessThanCppStringObj> groupMap;
	std::map<string, CMFCPropertyGridProperty*, LessThanCppStringObj> keyvalsMap;
	SelectedEntityList::const_iterator selEntsIt = selEnts.begin();
	for( ; selEntsIt != selEnts.end(); ++selEntsIt )
	{
		CEditorEntity *pEnt = *selEntsIt;
		Assert(pEnt);

		string classname = pEnt->GetClassnameA();
		CString classDescr(CA2T(classname.c_str()));
		CEntityRegisterEntity *pRegEnt = GetEntityRegister()->GetEntity(classname);
		if( pRegEnt )
			classDescr = CA2T(pRegEnt->GetDescribtion().c_str());
		else
			PostOutput( L"Unknown entity-classname %s", CA2T(classname.c_str()) );

		//has this class been handled before? if not create a new group for this one
		CMFCPropertyGridProperty *pPropGroup;
		auto groupMapIt = groupMap.find(classname);
		if( groupMapIt == groupMap.end() )
		{
			pPropGroup = new CMFCPropertyGridProperty(CString(classname.c_str()));
			m_wndPropList.AddProperty(pPropGroup);
			pPropGroup->SetDescription(classDescr);
			groupMap.insert(std::make_pair(classname, pPropGroup));
		}
		else
			pPropGroup = groupMapIt->second;

		const CEditorEntity::KeyValList *pEntKeyVals = pEnt->GetKeyVals();
		
		//Add all parameters from this entity directly
		for( auto it = pEntKeyVals->begin(); it != pEntKeyVals->end(); ++it )
		{
			string kvName = it->first;
			string kvValue = it->second;
			paramtypes_e kvType = paramtypes_e::PT_UNKNOWN;
			string kvDesc = "";
			CEntityKeyValue *regKv;
			if( pRegEnt && (regKv = pRegEnt->Get(kvName)) )
			{
				kvType = regKv->type;
				kvDesc = regKv->desc;
			}

			//is this parameter already there?
			auto keyvalsMapIt = keyvalsMap.find(kvName);
			if( keyvalsMapIt != keyvalsMap.end() )
			{
				CMFCPropertyGridProperty *prop = keyvalsMapIt->second;
				//move the kv to the common keyvals group if it's of a different class AND not already there
				if( wcscmp(prop->GetParent()->GetName(), CA2T(classname.c_str())) != 0 &&
					prop->GetParent() != commonKVsGroup )
				{
					prop->GetParent()->RemoveSubItem(prop, FALSE);
					commonKVsGroup->AddSubItem(prop);
				}
				if( prop->FormatProperty().Compare(CA2T(kvValue.c_str())) != 0 )
				{
					prop->SetData(1);
					prop->SetValue(_T("(multiple values)"));
				}
			}
			else
			{
				CMFCPropertyGridProperty *pProp = AddKeyVal(kvType, CA2T(kvName.c_str()), CA2T(kvValue.c_str()), CA2T(kvDesc.c_str()), pPropGroup);
				pProp->SetData(0);
				keyvalsMap.insert(std::make_pair(kvName, pProp));
			}
		}
	}

	//add non-empty groups and delete the empty one's
	if( commonKVsGroup->GetSubItemsCount() <= 0 )
		m_wndPropList.DeleteProperty(commonKVsGroup, FALSE, FALSE);

	for( auto it = groupMap.begin(); it != groupMap.end(); ++it )
		if( it->second->GetSubItemsCount() <= 0 )
			m_wndPropList.DeleteProperty(it->second, FALSE, FALSE);

	m_wndPropList.ExpandAll();
}


CMFCPropertyGridProperty *CPropertiesWnd::AddKeyVal(paramtypes_e type, const TCHAR *name,
	const TCHAR *value, const TCHAR *desc, CMFCPropertyGridProperty *parent)
{
	CMFCPropertyGridProperty *prop = NULL;
	
	switch(type)
	{
		case paramtypes_e::PT_ANGLE:
		case paramtypes_e::PT_ORIGIN:
		{
			//Remove a few places after the dot
			float x = 0.0f, y = 0.0f, z = 0.0f;
			_snwscanf_s(value, 256, _T("%f %f %f"), &x, &y, &z);
			wchar_t buf[256];
			_sntprintf_s(buf, 256, _T("%.2f %.2f %.2f"), x, y, z);
			prop = new CCustPropertyGridProperty(name, buf, desc, 0, NULL, NULL, _T(" +-.0123456789"));
			break;
		}
		case paramtypes_e::PT_COLOR:
		{
			int r = 255, g = 255, b = 255;
			float a = 1.0;
			_sntscanf_s(value, 256, _T("%i %i %i %f"), &r, &g, &b, &a);
			prop = new CCustColorProperty(name, RGB(r,g,b), a, NULL, desc);
			break;
		}
		case paramtypes_e::PT_UNKNOWN:
			prop = new CCustPropertyGridProperty(name, value, desc);
			break;
		default:
			prop = new CCustPropertyGridProperty(name, value, desc);
			PostOutput(L"Unhandled paramtype in CPropertiesWnd::AddKeyVal()");			
	}
	parent->AddSubItem(prop);
	return prop;
}


LRESULT CPropertiesWnd::OnPropertyChanged( WPARAM wparam, LPARAM lparam )
{
	//cast the lparam
	CMFCPropertyGridProperty *prop = (CMFCPropertyGridProperty*)lparam;
	Assert(prop);
	PostOutput(_T("Property \"%s\" changed to \"%s\""), prop->GetName(), prop->GetValue().bstrVal);

	UpdateProperty(prop);

	return 0;
}


void CPropertiesWnd::UpdateProperty(CMFCPropertyGridProperty *prop, bool hotUpdate)
{
	Assert(prop);
	Assert(GetActiveDocument());

	string key(CT2A(prop->GetName()));
	string val(CT2A(prop->GetValue().bstrVal));

	//Assign the new value to all selected entities
	const SelectedEntityList &selEnts = GetActiveDocument()->GetSelectedEntityList();

	for( auto itEnts = selEnts.begin(); itEnts != selEnts.end(); ++itEnts )
	{
		CEditorEntity *pEnt = *itEnts;
		if( pEnt->HasValue(key) )
			pEnt->SetValue(key, val);
	}

	if( !hotUpdate )
	{
		//TODO: undo queue
	}

	//GetActiveDocument()->UpdateAllViews(NULL);
}

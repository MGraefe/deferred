// editor/CEntityOutputWnd.cpp
// editor/CEntityOutputWnd.cpp
// editor/CEntityOutputWnd.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "editor.h"
#include "CEntityOutputWnd.h"
#include "afxdialogex.h"
#include <util/CEntityOutput.h>
#include "editorDoc.h"
#include "OutputWnd.h"
#include "CEntityRegister.h"
#include "CEditorEntity.h"
#include "CEditorEntityList.h"
#include <util/OutputSystem.h>



// CEntityOutputWnd-Dialogfeld

const TCHAR g_multiSelectionString[] = _T("(multiple values)");
const int CEntityOutputWnd::m_listSpacingBottom = 200;

IMPLEMENT_DYNAMIC(CEntityOutputWnd, CDialogEx)

CEntityOutputWnd::CEntityOutputWnd(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEntityOutputWnd::IDD, pParent)
{
	m_lastSizeX = -1;
	m_lastSizeY = -1;
	m_enableOutputToList = false;
}

CEntityOutputWnd::~CEntityOutputWnd()
{
}

void CEntityOutputWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTPUTS_LIST1, m_list);
	DDX_Control(pDX, IDC_OUTPUTS_COMBO3, m_cbTarget);
	DDX_Control(pDX, IDC_OUTPUTS_COMBO4, m_cbEvt);
	DDX_Control(pDX, IDC_OUTPUTS_COMBO2, m_cbInput);
	DDX_Control(pDX, IDC_OUTPUTS_COMBO1, m_cbParam);
	DDX_Control(pDX, IDC_OUTPUTS_EDIT1, m_eDelay);
	DDX_Control(pDX, IDC_OUTPUTS_CHECK1, m_bOnlyOnce);
	DDX_Control(pDX, IDC_OUTPUTS_BUTTON_ADD, m_bAdd);
	DDX_Control(pDX, IDC_OUTPUTS_BUTTON_COPY, m_bCopy);
	DDX_Control(pDX, IDC_OUTPUTS_BUTTON_PASTE, m_bPaste);
	DDX_Control(pDX, IDC_OUTPUTS_BUTTON_DELETE, m_bDelete);
}


BEGIN_MESSAGE_MAP(CEntityOutputWnd, CDialogEx)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_OUTPUTS_LIST1, &CEntityOutputWnd::OnLvnItemchanged)
	ON_CBN_EDITCHANGE(IDC_OUTPUTS_COMBO4, &CEntityOutputWnd::OnCbnEditchangeOutputsCombo4)
	ON_CBN_SELCHANGE(IDC_OUTPUTS_COMBO4, &CEntityOutputWnd::OnCbnSelchangeOutputsCombo4)
	ON_CBN_EDITCHANGE(IDC_OUTPUTS_COMBO3, &CEntityOutputWnd::OnCbnEditchangeOutputsCombo3)
	ON_CBN_SELCHANGE(IDC_OUTPUTS_COMBO3, &CEntityOutputWnd::OnCbnSelchangeOutputsCombo3)
	ON_CBN_EDITCHANGE(IDC_OUTPUTS_COMBO2, &CEntityOutputWnd::OnCbnEditchangeOutputsCombo2)
	ON_CBN_SELCHANGE(IDC_OUTPUTS_COMBO2, &CEntityOutputWnd::OnCbnSelchangeOutputsCombo2)
	ON_CBN_EDITCHANGE(IDC_OUTPUTS_COMBO1, &CEntityOutputWnd::OnCbnEditchangeOutputsCombo1)
	ON_CBN_SELCHANGE(IDC_OUTPUTS_COMBO1, &CEntityOutputWnd::OnCbnSelchangeOutputsCombo1)
	ON_EN_CHANGE(IDC_OUTPUTS_EDIT1, &CEntityOutputWnd::OnEnChangeOutputsEdit1)
	ON_BN_CLICKED(IDC_OUTPUTS_CHECK1, &CEntityOutputWnd::OnBnClickedOutputsCheck1)
	ON_BN_CLICKED(IDC_OUTPUTS_BUTTON_ADD, &CEntityOutputWnd::OnBnClickedOutputsButtonAdd)
	ON_BN_CLICKED(IDC_OUTPUTS_BUTTON_COPY, &CEntityOutputWnd::OnBnClickedOutputsButtonCopy)
	ON_BN_CLICKED(IDC_OUTPUTS_BUTTON_PASTE, &CEntityOutputWnd::OnBnClickedOutputsButtonPaste)
	ON_BN_CLICKED(IDC_OUTPUTS_BUTTON_DELETE, &CEntityOutputWnd::OnBnClickedOutputsButtonDelete)
	ON_CBN_KILLFOCUS(IDC_OUTPUTS_COMBO3, &CEntityOutputWnd::OnCbnKillfocusOutputsCombo3)
END_MESSAGE_MAP()


BOOL CEntityOutputWnd::PreTranslateMessage(MSG* pMsg)
{
	//if (pMsg->message == WM_KEYDOWN)
	//{
	//	if ((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE))
	//		pMsg->wParam = VK_TAB;
	//}
	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CEntityOutputWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	CRect rect;
	GetClientRect(rect);
	m_lastSizeX = rect.Width();
	m_lastSizeY = rect.Height();

	m_list.InitStyle();

	ArrangeChildren();

	m_cbTarget.FillData();
	ClearInputMask();
	EnableInputMask(FALSE);
	m_bAdd.EnableWindow(FALSE);
	m_bPaste.EnableWindow(FALSE);
	m_list.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CEntityOutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	ArrangeChildren();
}


void CEntityOutputWnd::UpdateList()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityOutputWnd::UpdateEntitySelection( CeditorDoc *activeDoc )
{
	bool entitySelected = !activeDoc->GetSelectedEntityList().empty();
	m_bAdd.EnableWindow(entitySelected);
	m_bPaste.EnableWindow(entitySelected && !theApp.GetOutputCopyPasteList().empty());
	m_list.EnableWindow(entitySelected);

	m_list.UpdateEntitySelection(activeDoc);
	ClearInputMask();
	m_cbTarget.FillData();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityOutputWnd::ArrangeChildren()
{
	CRect clientRect;
	GetClientRect(clientRect);

	if( m_lastSizeY > 0 && m_lastSizeX > 0 )
	{
		CRect listRect;
		m_list.GetWindowRect(listRect);
		ScreenToClient(listRect);

		//move all child windows in y direction
		int ofs = clientRect.Height() - m_lastSizeY;

		HWND child = ::GetWindow(GetSafeHwnd(), GW_CHILD | GW_HWNDFIRST);
		while( child )
		{
			if(child != m_list.GetSafeHwnd())
			{
				CRect childRect;
				::GetWindowRect(child, childRect);
				ScreenToClient(childRect);
				TCHAR classname[128];
				GetClassName(child, classname, 128);
				if( StrCmp(classname, _T("ComboBox")) == 0 ) //Scale combo-boxes
					childRect.right = max(clientRect.Width() - 10, 200);
				if( child == GetDlgItem(IDC_OUTPUTS_SETTINGS_GROUP)->GetSafeHwnd() ) //scale settings area
					childRect.right = max(clientRect.Width() - 5, 200);
				//ConsoleMessage("Child: %ls", classname);

				::SetWindowPos(child, 0, childRect.left, childRect.top + ofs, childRect.Width(), childRect.Height(),
					/*SWP_NOSIZE |*/ SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
			}
			child = ::GetWindow(child, GW_HWNDNEXT);
		}

		m_list.SetWindowPos(NULL, 5, 5,
			clientRect.Width() - listRect.left - 5,
			max(clientRect.Height() - m_listSpacingBottom - 5, 10), SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOACTIVATE);
	}

	m_lastSizeX = clientRect.Width();
	m_lastSizeY = clientRect.Height();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityOutputWnd::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if( (pNMLV->uChanged & LVIF_STATE)/* && 
		(pNMLV->uOldState & LVNI_SELECTED) && 
		!(pNMLV->uNewState & LVNI_SELECTED)*/ )
	{
		OnListSelectionChanged();
	}
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void UpdateStringMultiSelection(bool first, CString &str, const CString &newSelection)
{
	if( !first )
	{
		if(str != newSelection)
			str = g_multiSelectionString;
	}
	else
		str = newSelection;			
}


bool IsMultiSelection(const TCHAR *str)
{
	return StrCmp(str, g_multiSelectionString) == 0;
}


void SetComboBoxText(CCustomComboBox &cb, const TCHAR *text)
{
	if(IsMultiSelection(text))
		cb.SetFontData(false, RGB(100, 100, 100));
	else
		cb.SetFontData(false, RGB(0, 0, 0), CCB_NOWEIGHT);
	cb.SetWindowText(text);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityOutputWnd::OnListSelectionChanged()
{
	CString evt;
	CString target;
	CString input;
	CString param;
	CString delay;
	CString onlyOnce;

	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if( pos )
	{
		bool first = true;
		while(pos)
		{
			int nItem = m_list.GetNextSelectedItem(pos);
			UpdateStringMultiSelection(first, evt, m_list.GetItemText(nItem, COutputListWnd::SUBITEM_EVENT));
			UpdateStringMultiSelection(first, target, m_list.GetItemText(nItem, COutputListWnd::SUBITEM_TARGET));
			UpdateStringMultiSelection(first, input, m_list.GetItemText(nItem, COutputListWnd::SUBITEM_INPUT));
			UpdateStringMultiSelection(first, param, m_list.GetItemText(nItem, COutputListWnd::SUBITEM_PARAM));
			UpdateStringMultiSelection(first, delay, m_list.GetItemText(nItem, COutputListWnd::SUBITEM_DELAY));
			UpdateStringMultiSelection(first, onlyOnce, m_list.GetItemText(nItem, COutputListWnd::SUBITEM_ONLYONCE));
			first = false;
		}

		EnableInputMask(TRUE);
		m_enableOutputToList = false;
		SetComboBoxText(m_cbEvt, evt);
		SetComboBoxText(m_cbTarget, target);
		SetComboBoxText(m_cbInput, input);
		SetComboBoxText(m_cbParam, param);
		m_eDelay.SetWindowText(delay);
		UpdateTargetComboBox();
		UpdateEventComboBox();
		UpdateInputComboBox();
		m_bOnlyOnce.SetCheck((onlyOnce.IsEmpty() || onlyOnce == _T("no")) ? BST_UNCHECKED : onlyOnce == _T("yes") ? BST_CHECKED : BST_INDETERMINATE);
		m_enableOutputToList = true;
	}
	else
	{
		m_enableOutputToList = false;
		ClearInputMask();
		EnableInputMask(FALSE);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void GetComboBoxString(CComboBox *box, TCHAR *buf, int bufSize)
{
	int sel = box->GetCurSel();
	if( sel == CB_ERR )
		box->GetWindowText(buf, bufSize);
	else
	{
		if(box->GetLBTextLen(sel)+1 > bufSize)
			StrCpy(buf, _T("Too Long..."));
		else
			box->GetLBText(sel, buf);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityOutputWnd::UpdateToList()
{
	if( !m_enableOutputToList )
		return;

	TCHAR buf[128];

	GetComboBoxString(&m_cbEvt, buf, 128);
	if( !IsMultiSelection(buf) )
		m_list.SetOutputEvent(buf);
	
	GetComboBoxString(&m_cbTarget, buf, 128);
	if( !IsMultiSelection(buf) )
		m_list.SetOutputTarget(buf);

	GetComboBoxString(&m_cbInput, buf, 128);
	if( !IsMultiSelection(buf) )
		m_list.SetOutputInput(buf);

	GetComboBoxString(&m_cbParam, buf, 128);
	if( !IsMultiSelection(buf) )
		m_list.SetOutputParam(buf);

	m_eDelay.GetWindowText(buf, 128);
	if( !IsMultiSelection(buf) )
		m_list.SetOutputDelay(buf);

	int onlyOnce = m_bOnlyOnce.GetCheck();
	if( onlyOnce != BST_INDETERMINATE )
		m_list.SetOutputOnlyOnce(onlyOnce == BST_CHECKED);

	m_list.UpdateSelected();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityOutputWnd::OnCbnEditchangeOutputsCombo4()
{
	UpdateToList();
}

void CEntityOutputWnd::OnCbnSelchangeOutputsCombo4()
{
	UpdateToList();
}

void CEntityOutputWnd::OnCbnEditchangeOutputsCombo3()
{
	UpdateToList();
	UpdateTargetComboBox();
}

void CEntityOutputWnd::OnCbnSelchangeOutputsCombo3()
{
	UpdateToList();
	UpdateTargetComboBox();
}

void CEntityOutputWnd::OnCbnEditchangeOutputsCombo2()
{
	UpdateToList();
}

void CEntityOutputWnd::OnCbnSelchangeOutputsCombo2()
{
	UpdateToList();
}

void CEntityOutputWnd::OnCbnEditchangeOutputsCombo1()
{
	UpdateToList();
}

void CEntityOutputWnd::OnCbnSelchangeOutputsCombo1()
{
	UpdateToList();
}

void CEntityOutputWnd::OnEnChangeOutputsEdit1()
{
	UpdateToList();
}

void CEntityOutputWnd::OnBnClickedOutputsCheck1()
{
	UpdateToList();
}

void CEntityOutputWnd::OnBnClickedOutputsButtonAdd()
{
	m_list.DeselectAll();
	ClearInputMask();
	m_list.StartNewOutput();
}

void CEntityOutputWnd::OnCbnKillfocusOutputsCombo3()
{
	UpdateInputComboBox();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityOutputWnd::ClearInputMask()
{
	m_cbEvt.SetCurSel(-1);
	m_cbTarget.SetCurSel(-1);
	m_cbInput.SetCurSel(-1);
	m_cbParam.SetCurSel(-1);
	m_eDelay.SetWindowText(_T(""));
	m_bOnlyOnce.SetCheck(BST_UNCHECKED);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityOutputWnd::EnableInputMask( BOOL bEnable /*= TRUE*/ )
{
	m_cbEvt.EnableWindow(bEnable);
	m_cbTarget.EnableWindow(bEnable);
	m_cbInput.EnableWindow(bEnable);
	m_cbParam.EnableWindow(bEnable);
	m_eDelay.EnableWindow(bEnable);
	m_bOnlyOnce.EnableWindow(bEnable);
	m_bDelete.EnableWindow(bEnable);
	m_bCopy.EnableWindow(bEnable);
}



void CEntityOutputWnd::OnBnClickedOutputsButtonCopy()
{
	if( m_list.CopyOutputs() )
		m_bPaste.EnableWindow(TRUE);
}


void CEntityOutputWnd::OnBnClickedOutputsButtonPaste()
{
	m_list.PasteOutputs();
}


void CEntityOutputWnd::OnBnClickedOutputsButtonDelete()
{
	m_list.DeleteOutputs();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityOutputWnd::UpdateEventComboBox()
{
	CString oldText;
	m_cbEvt.GetWindowText(oldText);

	m_cbEvt.ResetContent();

	for(int id : m_list.GetEditIds()) //iterate over selected outputs
	{
		COutputListWnd::OutputSourceList *sourceList = m_list.GetSourceEnts(id);
		for(const std::pair<CEditorEntity*, CEntityOutput*> &outputPair : *sourceList) //iterate over output source entities
		{
			CEditorEntity *ent = outputPair.first;
			CEntityRegisterEntity *regEnt = GetEntityRegister()->GetEntity(ent->GetClassnameA());
			if(regEnt)
			{
				size_t numOuts = regEnt->GetNumOutputs();
				for(size_t i = 0; i < numOuts; i++) //iterate over the possible outputs of this source entity
				{
					const CEntityRegisterOutput *regOut = regEnt->GetOutput(i);
					CString outName(CA2T(regOut->name.c_str()));
					if( m_cbEvt.FindString(-1, outName) == CB_ERR )
						m_cbEvt.AddString(outName);
				}
			}				
		}
	}

	m_cbEvt.SetWindowText(oldText);
}

void CEntityOutputWnd::AddInputsForClassname(const std::string &classname)
{
	CEntityRegisterEntity *regEnt = GetEntityRegister()->GetEntity(classname);
	if( regEnt )
	{
		size_t numIns = regEnt->GetNumInputs();
		for(size_t i = 0; i < numIns; i++) //iterate over possible inputs
		{
			CString inName(CA2T(regEnt->GetInput(i)->name.c_str()));
			if( m_cbInput.FindString(-1, inName) == CB_ERR )
				m_cbInput.AddString(inName);
		}
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityOutputWnd::UpdateInputComboBox()
{
	CString oldText;
	m_cbInput.GetWindowText(oldText);

	m_cbInput.ResetContent();

	for(int id : m_list.GetEditIds()) //iterate over selected outputs
	{
		COutputListWnd::OutputSourceList *list = m_list.GetSourceEnts(id);
		//target name is the same for all source outputs so this is safe
		const std::string &targetName = list->at(0).second->getTarget();
		OutputSystem::genericTarget_e target = OutputSystem::getGenericTarget(CA2T(targetName.c_str()));

		if( target == OutputSystem::TARGET_SELF )
		{
			//Add inputs of ourselves
			for(const std::pair<CEditorEntity*, CEntityOutput*> &p: *list)
				AddInputsForClassname(p.first->GetClassnameA());
		}
		else if( target == OutputSystem::TARGET_ACTIVATOR )
		{
			//Add all available input functions
			std::vector<std::string> classnames;
			GetEntityRegister()->GetEntityNames(classnames);
			for(const std::string &classname : classnames)
				AddInputsForClassname(classname);
		}
		else if( target == OutputSystem::TARGET_ERROR ) //is not of generic target type
		{
			//Add input functions of target entities
			std::vector<CEditorEntity*> targets = GetActiveDocument()->GetEntityList()->FindByName(targetName);
			for(CEditorEntity *ent : targets)
				AddInputsForClassname(ent->GetClassnameA());
		}
		else
		{
			AssertMsg(false, "Added a new generic output-target?");
		}
	}

	m_cbInput.SetWindowText(oldText);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityOutputWnd::UpdateTargetComboBox()
{
	TCHAR buf[256];
	GetComboBoxString(&m_cbTarget, buf, 256);
	if(IsMultiSelection(buf))
	{
		m_cbTarget.SetFontData(false, 0, CCB_NOCOLOR);
	}
	else
	{
		int num = GetActiveDocument()->GetEntityList()->FindNumberByName(std::string(CT2A(buf)));
		bool valid = num > 1 || OutputSystem::isGenericTargetName(buf);
		m_cbTarget.SetFontData(num > 1, valid ? RGB(0, 0, 0) : RGB(200, 0, 0));
	}
}


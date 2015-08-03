// editor/COutputListWnd.cpp
// editor/COutputListWnd.cpp
// editor/COutputListWnd.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"

#include "COutputListWnd.h"
#include "editorDoc.h"
#include "CEditorEntity.h"
#include "CEditorEntityList.h"
#include "CEditorEntity.h"
#include "resource.h"
#include "editor.h"
#include <util/OutputSystem.h>

COutputListWnd::COutputListWnd() :
	CListCtrl()
{
	m_imageList = new CImageList();
	if( !m_imageList->Create(IDB_INOUT_ICONS, 16, 4, RGB(255, 255, 255)) )
		error("Error loading bitmap IDB_INOUT_ICONS");
}


COutputListWnd::~COutputListWnd()
{

}


void COutputListWnd::InitStyle()
{
	SetImageList(m_imageList, LVSIL_SMALL);
	
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES /*| LVS_SHOWSELALWAYS*/);

	int columnSizes[7] = {20, 100, 100, 100, 100, 40, 40};
	TCHAR *columnNames[7] = {_T(""), _T("Event"), _T("Target"), _T("Target Input"), _T("Parameter"), _T("Delay"), _T("Only Once")};
	for(int i = 0; i < 7; i++)
	{
		LVCOLUMN column;
		column.mask = LVCF_TEXT | LVCF_WIDTH;
		column.pszText = columnNames[i];
		column.cx = columnSizes[i];		
		InsertColumn(i, &column);
	}
}


void COutputListWnd::UpdateEntitySelection(CeditorDoc *activeDoc)
{
	DeselectAll();
	DeleteAll();

	if(activeDoc)
	{
		//gather identical outputs
		std::map<CEntityOutput, OutputSourceList*> sameOutputMap;
		for(CEditorEntity *ent : activeDoc->GetSelectedEntityList())
		{
			for(CEntityOutput *output : ent->GetOutputs())
			{
				auto it = sameOutputMap.find(*output);
				if( it == sameOutputMap.end() )
					it = sameOutputMap.insert(std::make_pair(*output, new OutputSourceList())).first;
				it->second->push_back(std::make_pair(ent, output));
			}
		}

		//insert list items and add entity sources
		for(auto it = sameOutputMap.begin(); it != sameOutputMap.end(); ++it)
		{
			int itemId = AddOutput(&it->first);
			SetItemData(itemId, (DWORD)(it->second));
		}
	}
}


int COutputListWnd::AddOutput(const CEntityOutput *output)
{
	int iconId = GetActiveDocument() && GetActiveDocument()->GetEntityList()->FindNumberByName(output->getTarget()) ? 0 : 1;
	int itemId = InsertItem(GetItemCount(), _T(""), iconId);
	SetItemData(itemId, 0);
	SetItemText(itemId, 1, CA2T(output->getEvent().c_str()));
	SetItemText(itemId, 2, CA2T(output->getTarget().c_str()));
	SetItemText(itemId, 3, CA2T(output->getInput().c_str()));
	SetItemText(itemId, 4, CA2T(output->getParam().c_str()));

	CString str;
	str.Format(_T("%.3f"), output->getDelay());
	SetItemText(itemId, 5, str);

	SetItemText(itemId, 6, output->getOnlyOnce() ? _T("yes") : _T("no"));
	return itemId;
}


int COutputListWnd::StartNewOutput()
{
	OutputSourceList *sourceList = new OutputSourceList();
	for(CEditorEntity *ent : GetActiveDocument()->GetSelectedEntityList())
	{
		CEntityOutput *newOutput = new CEntityOutput();
		ent->AddOutput(newOutput);
		sourceList->push_back(std::make_pair(ent, newOutput));
	}

	int item = InsertItem(GetItemCount(), _T(""), 1);
	SetItemData(item, (DWORD_PTR)sourceList);
	Select(item);
	return item;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::SetOutputEvent( const std::string &evt )
{
	SetOutputEvent(CA2T(evt.c_str()));
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::SetOutputEvent( const TCHAR *evt )
{
	if(!m_editIds.empty())
	{
		for(int id : m_editIds)
			SetItemText(id, 1, evt);
		UpdateOutputIcon();
		SyncSelectionWithEntity();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::SetOutputTarget( const std::string &target )
{
	SetOutputTarget(CA2T(target.c_str()));
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::SetOutputTarget( const TCHAR *target )
{
	if(!m_editIds.empty())
	{
		for(int id : m_editIds)
			SetItemText(id, 2, target);
		UpdateOutputIcon();
		SyncSelectionWithEntity();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::SetOutputInput( const std::string &input )
{
	SetOutputInput(CA2T(input.c_str()));
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::SetOutputInput( const TCHAR *input )
{
	if(!m_editIds.empty())
	{
		for(int id : m_editIds)
			SetItemText(id, 3, input);
		UpdateOutputIcon();
		SyncSelectionWithEntity();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::SetOutputParam( const std::string &param )
{
	SetOutputParam(CA2T(param.c_str()));
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::SetOutputParam( const TCHAR *param )
{
	if(!m_editIds.empty())
	{
		for(int id : m_editIds)
			SetItemText(id, 4, param);
		UpdateOutputIcon();
		SyncSelectionWithEntity();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::SetOutputDelay( float delay )
{
	if(!m_editIds.empty())
	{
		CString strDelay;
		strDelay.Format(_T("%.3f"), delay);
		for(int id : m_editIds)
			SetItemText(id, 5, strDelay);
		UpdateOutputIcon();
		SyncSelectionWithEntity();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::SetOutputDelay( const TCHAR *delay )
{
	float fDelay = (float)_ttof(delay);
	SetOutputDelay(fDelay);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::SetOutputOnlyOnce( bool onlyOnce )
{
	if(!m_editIds.empty())
	{
		for(int id : m_editIds)
			SetItemText(id, 6, onlyOnce ? _T("yes") : _T("no"));
		UpdateOutputIcon();
		SyncSelectionWithEntity();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::UpdateOutputIcon()
{
	for(int id : m_editIds)
	{
		int iconId = IsTargetValid(GetItemText(id, SUBITEM_TARGET)) ? 0 : 1;

		LVITEM item;
		item.iItem = id;
		item.iSubItem = 0;
		item.mask = LVIF_IMAGE;
		item.iImage = iconId;
		SetItem(&item);
	}
}

BEGIN_MESSAGE_MAP(COutputListWnd, CListCtrl)
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &COutputListWnd::OnLvnItemchanged)
END_MESSAGE_MAP()


BOOL COutputListWnd::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if( (pNMLV->uChanged & LVIF_STATE) )
	{
		m_editIds.clear();
		POSITION pos = GetFirstSelectedItemPosition();
		while(pos)
		{
			int nItem = GetNextSelectedItem(pos);
			m_editIds.push_back(nItem);
		}
	}

	return FALSE; //False in return of ON_NOTIFY_REFLECT_EX means that this message is handled by the parent also
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::UpdateSelected()
{
	for(int id : m_editIds)
		Update(id);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::DeselectAll()
{
	for(int i = 0; i < GetItemCount(); i++)
		Deselect(i);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::Select( int item )
{
	SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
	SetSelectionMark(item);
	EnsureVisible(item, TRUE);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::Deselect( int item )
{
	SetItemState(item, ~LVIS_SELECTED, LVIS_SELECTED);
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
COutputListWnd::OutputSourceList *COutputListWnd::GetSourceEnts( int editId )
{
	static_assert(sizeof(DWORD_PTR) == sizeof(void*), "Invalid data size");
	DWORD_PTR data = GetItemData(editId);
	OutputSourceList *list = (OutputSourceList*)data;
	return list;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::SyncSelectionWithEntity()
{
	bool bDocumentEdited = false;

	for(int id : m_editIds)
	{
		const OutputSourceList *outList = GetSourceEnts(id);
		for(const std::pair<CEditorEntity*, CEntityOutput*> &source : *outList)
		{
			CEntityOutput *output = source.second;
			std::string evt = GetItemTextStringA(id, SUBITEM_EVENT);
			std::string target = GetItemTextStringA(id, SUBITEM_TARGET);
			std::string input = GetItemTextStringA(id, SUBITEM_INPUT);
			std::string param = GetItemTextStringA(id, SUBITEM_PARAM);
			float delay = (float)atof(GetItemTextStringA(id, SUBITEM_DELAY).c_str());
			bool onlyOnce = GetItemText(id, SUBITEM_ONLYONCE) == _T("yes");

			bDocumentEdited = bDocumentEdited || 
				output->getEvent() != evt || output->getTarget() != target || output->getInput() != input ||
				output->getParam() != param || output->getDelay() != delay || output->getOnlyOnce() != onlyOnce;

			output->setEvent(evt);
			output->setTarget(target);
			output->setInput(input);
			output->setParam(param);
			output->setDelay(delay);
			output->setOnlyOnce(onlyOnce);
		}
	}

	if( bDocumentEdited )
		GetActiveDocument()->SetModified(true);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::string COutputListWnd::GetItemTextStringA( int itemId, SubItemIndex subItem )
{
	return std::string(CT2A(this->GetItemText(itemId, subItem)));
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::DeleteAll()
{
	for(int i = 0; i < GetItemCount(); i++)
	{
		const OutputSourceList *list = GetSourceEnts(i);
		if(list)
			delete list;
	}
	DeleteAllItems();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool COutputListWnd::OutputValid( int editId )
{
	std::string target = CT2A(GetItemText(editId, SUBITEM_TARGET));
	if( !target.empty() &&
		GetActiveDocument() &&
		GetActiveDocument()->GetEntityList()->FindNumberByName(target) > 0 )
		return true;
	return false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::PasteOutputs()
{
	if(!GetActiveDocument() || GetActiveDocument()->GetSelectedEntityList().empty() )
		return;

	DeselectAll();
	std::vector<int> pastedItems;
	for(const CEntityOutput &output : theApp.GetOutputCopyPasteList())
	{
		int item = StartNewOutput();
		SetOutputEvent(output.getEvent());
		SetOutputTarget(output.getTarget());
		SetOutputInput(output.getInput());
		SetOutputParam(output.getParam());
		SetOutputDelay(output.getDelay());
		SetOutputOnlyOnce(output.getOnlyOnce());
		pastedItems.push_back(item);
		Deselect(item);
	}

	for(int id : pastedItems)
		Select(id);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool COutputListWnd::CopyOutputs()
{
	if(!GetActiveDocument() || GetActiveDocument()->GetSelectedEntityList().empty() || m_editIds.empty())
		return false;

	theApp.GetOutputCopyPasteList().clear();
	for(int id : m_editIds)
	{
		const CEntityOutput *output = GetSourceEnts(id)->at(0).second;
		theApp.GetOutputCopyPasteList().push_back(CEntityOutput(*output));
	}

	return !theApp.GetOutputCopyPasteList().empty();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COutputListWnd::DeleteOutputs()
{
	if(!GetActiveDocument() || GetActiveDocument()->GetSelectedEntityList().empty() || m_editIds.empty())
		return;
	for(int id : m_editIds)
	{
		OutputSourceList *list = GetSourceEnts(id);
		for(std::pair<CEditorEntity*, CEntityOutput*> &sourcePair : *list)
		{
			sourcePair.first->RemoveOutput(sourcePair.second);
			delete sourcePair.second;
		}
		delete list;
	}

	int delCount = 0;
	std::vector<int> editIdsCopy = m_editIds;
	std::sort(editIdsCopy.begin(), editIdsCopy.end());
	for(int id : editIdsCopy)
	{
		DeleteItem(id - delCount);
		delCount++;
	}

	m_editIds.clear();
	DeselectAll();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool COutputListWnd::IsTargetValid(const TCHAR *target) const
{
	CeditorDoc *doc = GetActiveDocument();
	if( !doc )
		return false;
	if( OutputSystem::isGenericTargetName(target) )
		return true;
	if( doc->GetEntityList()->FindNumberByName(std::string(CT2A(target))) > 0 )
		return true;
	return false;
}
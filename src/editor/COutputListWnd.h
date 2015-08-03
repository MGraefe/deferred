// editor/COutputListWnd.h
// editor/COutputListWnd.h
// editor/COutputListWnd.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__editor__COutputListWnd_H__
#define deferred__editor__COutputListWnd_H__

#include <afxcmn.h>

class CEntityOutput;
class CeditorDoc;
class CEditorEntity;

class COutputListWnd : public CListCtrl
{
	//DECLARE_DYNAMIC(COutputListWnd)

public:
	//Sub item indices
	enum SubItemIndex
	{
		SUBITEM_ICON = 0,
		SUBITEM_EVENT,
		SUBITEM_TARGET,
		SUBITEM_INPUT,
		SUBITEM_PARAM,
		SUBITEM_DELAY,
		SUBITEM_ONLYONCE,
	};

	typedef std::vector<std::pair<CEditorEntity*, CEntityOutput*> > OutputSourceList;

	COutputListWnd();
	virtual ~COutputListWnd();

	void DeleteAll();
	void DeselectAll();
	void Select(int item);
	void Deselect(int item);

	void InitStyle();
	void UpdateEntitySelection(CeditorDoc *activeDoc);
	int  AddOutput(const CEntityOutput *output);
	int StartNewOutput();
	void SetOutputEvent(const std::string &evt);
	void SetOutputEvent(const TCHAR *evt);
	void SetOutputTarget(const std::string &target);
	void SetOutputTarget(const TCHAR *target);
	void SetOutputInput(const std::string &input);
	void SetOutputInput(const TCHAR *input);
	void SetOutputParam(const std::string &param);
	void SetOutputParam(const TCHAR *param);
	void SetOutputDelay(float delay);
	void SetOutputDelay(const TCHAR *delay);
	void SetOutputOnlyOnce(bool onlyOnce);

	void UpdateOutputIcon();
	void UpdateSelected();
	void PasteOutputs();
	bool CopyOutputs();
	void DeleteOutputs();

	void SyncSelectionWithEntity();

	OutputSourceList *GetSourceEnts(int editId);
	std::string GetItemTextStringA( int itemId, SubItemIndex subItem );
	inline const std::vector<int> &GetEditIds() const { return m_editIds; }

	bool IsTargetValid(const TCHAR *target) const;

private:
	std::vector<int> m_editIds;
	CImageList *m_imageList;
	bool OutputValid(int editId);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif // deferred__editor__COutputListWnd_H__
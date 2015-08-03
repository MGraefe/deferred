// editor/CEditorEntityOutputList.h
// editor/CEditorEntityOutputList.h
// editor/CEditorEntityOutputList.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__editor__CEditorEntityOutputList_H__
#define deferred__editor__CEditorEntityOutputList_H__

#include <util/CEntityOutput.h>

class CEditorEntityOutputList
{
public:
	typedef std::vector<CEntityOutput*> container_type;
	typedef container_type::iterator iterator;
	typedef container_type::const_iterator const_iterator;
	typedef container_type::value_type value_type;
	typedef container_type::reference reference;
	typedef container_type::const_reference const_reference;
	typedef container_type::size_type size_type;

public:
	CEditorEntityOutputList();
	CEditorEntityOutputList(const CEditorEntityOutputList &other);
	~CEditorEntityOutputList();

	CEditorEntityOutputList &operator=(const CEditorEntityOutputList &other);

	inline iterator begin() { return m_data.begin(); }
	inline const_iterator begin() const { return m_data.begin(); }
	inline iterator end() { return m_data.end(); }
	inline const_iterator end() const { return m_data.end(); }
	inline reference operator[](size_type i) { return m_data[i]; }
	inline const_reference operator[](size_type i) const { return m_data[i]; }
	inline bool empty() const { return m_data.empty(); }
	inline size_type size() const { return m_data.size(); }
	inline void push_back(const_reference v) { m_data.push_back(v); }
	inline void pop_back() { m_data.pop_back(); }
	inline reference front() { return m_data.front(); }
	inline const_reference front() const { return m_data.front(); }
	inline reference back() { return m_data.back(); }
	inline const_reference back() const { return m_data.back(); }
	inline iterator erase(const_iterator position) { return m_data.erase(position); }
	inline iterator erase(const_iterator from, const_iterator to) { return m_data.erase(from, to); }

	static void CloneOutputList(const CEditorEntityOutputList &from, CEditorEntityOutputList &to);

private:
	void cleanup();
	container_type m_data;
};

#endif // deferred__editor__CEditorEntityOutputList_H__

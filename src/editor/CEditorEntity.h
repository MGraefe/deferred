// editor/CEditorEntity.h
// editor/CEditorEntity.h
// editor/CEditorEntity.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CEditorEntity.h

#pragma once
#ifndef deferred__editor__CEditorEntity_H__
#define deferred__editor__CEditorEntity_H__

#include "tracelineinfo.h"

#include <ostream>
#include <util/CScriptParser.h>
#include "CSprite.h"
#include <util/CEntityOutput.h>
#include "CEditorEntityOutputList.h"

class IEditorEntityFactory;

#define DECLARE_EDITORENT(baseclass, thisclass) public: \
	virtual thisclass *CreateNew(int index) const { return new thisclass(GetClassnameA(), index); } \
	virtual thisclass *Clone() const { return new thisclass(*this); } \
	virtual thisclass *Copy(int index) const { return new thisclass(*this, index); }

class CEditorEntity
{
	DECLARE_EDITORENT(CEditorEntity, CEditorEntity)
	friend class COutputListWnd;
	friend class CUndoRedoStateOutputListChanged;

public:
	typedef std::map<std::string, std::string, LessThanCppStringObj> KeyValList;
	typedef CEditorEntityOutputList OutputList;
	typedef std::ostream outstr;

public:
	CEditorEntity( const CScriptSubGroup *pGrp, int index );
	CEditorEntity( const std::string &classname, int index );
	CEditorEntity( const CEditorEntity &other, int index );
	virtual ~CEditorEntity();
	//CEditorEntity( const CEditorEntity &ent ) : m_index(-1) { } //DUMMY CONSTRUCTOR, DO NOT USE
public:
	virtual void Render( void );
	bool IsSelected( void );

	Vector3f GetAbsCenter(void) const { return m_center; }
	void SetAbsCenter( const Vector3f &center );
	Angle3d GetAbsAngles(void) const { return m_angles; } 
	void SetAbsAngles( const Angle3d &angles );
	std::string GetClassnameA(void) const { return m_classname; }
	std::wstring GetClassnameW(void) const { return std::wstring( CA2W(m_classname.c_str()) ); }

	//Overloadables
	virtual void Serialize( outstr &str ) const;
	virtual bool Deserialize( const CScriptSubGroup *pGrp );
	virtual bool TraceLine( tracelineinfo_t &info );
	virtual void OnCreate( void );
	virtual void OnDestroy( void );
	virtual void OnParamChanged( const std::string &key, 
		const std::string &oldval, 
		const std::string &newval );
	virtual std::string GetSerializationHeaderString( void ) const;

	bool IsTranslatable( void ) const;
	bool IsRotatable( void ) const;

	std::string GetName( void ) const;
	std::string GetValue( const std::string &key );
	inline const KeyValList *GetKeyVals() { return &m_keyvals; }

	inline const OutputList &GetOutputs() const { return m_outputs; }
	inline CEntityOutput *GetOutput(size_t i) const { return m_outputs[i]; }
	inline void AddOutput(CEntityOutput *output) { m_outputs.push_back(output); }
	void RemoveOutput(const CEntityOutput *output);
	void RemoveOutput(const CEntityOutput &output);
	inline void RemoveOutput(size_t i) { m_outputs.erase(m_outputs.begin()+i);}
	inline bool HasOutput(const std::string &evt) const
	{ 
		for(const CEntityOutput *output : m_outputs )
			if(output->getEvent() == evt)
				return true;
		return false;
	}
	void SetOutputList(const OutputList &outputs);


	//Do not call from OnParamChanged because of resulting recursion.
	void SetValue( const std::string &key, const std::string &val ); 
	bool HasValue( const std::string &key );

	void SetSpriteTexture( const std::string &tex );
	
	int GetIndex( void ) { return m_index; }

	void UpdateZVal(const Vector3f &camPos, const Vector3f &camDir);
	inline float GetZVal() const { return m_zVal; }


	template<typename InputIterator>
	static Vector3f GetEntitiesCenter(InputIterator begin, InputIterator end)
	{
		Vector3f center = vec3_null;
		size_t count = 0;
		for(InputIterator it = begin; it != end; ++it)
		{
			count++;
			center += (*it)->GetAbsCenter();
		}
		
		if(count > 0)
			center /= (float)count;

		return center;
	}

private:
	inline OutputList &GetOutputs() { return m_outputs; }
	void AddDefaultParams(void);
	void InitTranslatableRotatable(void);

protected:
	Vector3f	m_center;
	Angle3d		m_angles;
	const int	m_index;
	KeyValList	m_keyvals;
	OutputList  m_outputs;
	std::string m_classname;
	std::string m_spriteName;
	CSprite*	m_sprite;
	bool		m_bTranslatable;
	bool		m_bRotatable;
	float		m_zVal;
};


#endif // deferred__editor__CEditorEntity_H__

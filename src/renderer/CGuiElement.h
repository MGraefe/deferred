// renderer/CGuiElement.h
// renderer/CGuiElement.h
// renderer/CGuiElement.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CGuiElement_H__
#define deferred__renderer__CGuiElement_H__

#include <vector>
#include <util/maths.h>
#include <string>
#include <util/CScriptParser.h>
#include <util/dll.h>
#include <map>
#include <string>

#define ASSERT_CHILD_VECTOR(i) Assert( ((i) >= 0) && ((i) < m_Children.size()) )

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING

namespace gui
{


enum anchor_x_e
{
	ANCHOR_LEFT,
	ANCHOR_RIGHT,
};


enum anchor_y_e
{
	ANCHOR_TOP,
	ANCHOR_BOTTOM,
};

class Anchors
{
public:
	Anchors(anchor_x_e left = ANCHOR_LEFT, anchor_y_e top = ANCHOR_TOP,
		anchor_x_e right = ANCHOR_LEFT, anchor_y_e bottom = ANCHOR_TOP) :
		left(left),
		top(top),
		right(right),
		bottom(bottom)
	{}

public:
	anchor_x_e left;
	anchor_y_e top;
	anchor_x_e right;
	anchor_y_e bottom;
};


class AnchorDists
{
public:
	AnchorDists(float left = 0.0f, float top = 0.0f, float right = 0.0f, float bottom = 0.0f) :
		left(left),
		top(top),
		right(right),
		bottom(bottom)
	{}

public:
	float left;
	float top;
	float right;
	float bottom;
};


class DLL CGuiElement
{
public:
	CGuiElement();
	virtual ~CGuiElement();

	inline CGuiElement *GetParent( void ) { return m_pParent; }
	inline void SetParent( CGuiElement *pParent ) { m_pParent = pParent; }
	
	//Get position as specified in script
	inline const Vector3f &GetPosition( void ) const { return m_vPos; }
	void SetInitPos(const Vector3f &pos );
	inline const Vector3f &GetInitPos( void ) const { return m_vInitPos; }
	
	inline void SetInitSize( const Vector2f &vSize ) { SetInitSize(vSize.x, vSize.y); }
	void SetInitSize( float x, float y );
	inline const Vector2f &GetSize( void ) const { return m_vSize; }
	inline const Vector2f &GetInitSize( void ) const { return m_vInitSize; }
	inline float GetSizeX( void ) const { return m_vSize.x; }
	inline float GetSizeY( void ) const { return m_vSize.y; }
	Vector2f GetParentSize();

	//Recalculate rendered position
	void UpdatePosition(bool recursive = true);

	void SetAnchors(const Anchors &anchors, bool updatePosition = true);
	inline const Anchors &GetAnchors() const { return m_anchors; }

	inline const AnchorDists &GetInitialAnchorDists() const { return m_initialAnchorDists; }

	inline void SetName( const std::string &name ) { m_Name = name; }
	inline const std::string &GetName( void ) const { return m_Name; }
	
	virtual bool IsVisible() { return true; }

	bool IsPointInside(const Vector2f &relativePoint) const { 
		return relativePoint.x >= 0.0f && relativePoint.y >= 0.0f && 
			relativePoint.x <= GetSizeX() && relativePoint.y <= GetSizeY(); 
	}

	//Child handling
	inline size_t GetNumberOfChildren( void ) const { return m_Children.size(); }
	inline CGuiElement *GetChild( size_t i ) const { ASSERT_CHILD_VECTOR(i); return m_Children[i]; }
	inline const std::vector<CGuiElement*> &GetChildren() const { return m_Children; }

	void AddChild( CGuiElement *pChild );
	void RemoveChild( CGuiElement *pChild );
	void RemoveChild( size_t index );
	void RemoveAndDeleteChild( CGuiElement *pChild );
	void RemoveAndDeleteChild( size_t index );
	bool IsChild( CGuiElement *pChild );
	int FindChild( CGuiElement *pChild );

	//Rendering
	void DrawQuad( float x1, float y1, float x2, float y2 );
	void DrawTriangle( const Vector2f &v1, const Vector2f &v2, const Vector2f &v3 );
	void SetDrawColor( float r, float g, float b, float a = 1.0f );
	void SetDrawColor( BYTE r, BYTE g, BYTE b, BYTE a = 255 );
	void SetDrawColor( Color c );
	void Render( void );
	void TransformStart( const Vector3f &vPos );
	void TransformEnd( void );


	//Overloadables
	virtual void VParseAdditionalVars( CScriptSubGroup *vars ) { }
	virtual void VInit( void ) { }
	virtual void VRenderCustom( void );
	virtual void VRenderBackground( void ) { }
	
private:
	void UpdateAnchorDists();

	std::string m_Name;
	CGuiElement *m_pParent;
	std::vector<CGuiElement*> m_Children;
	Vector3f m_vPos;
	Vector3f m_vInitPos;
	Vector2f m_vSize;
	Vector2f m_vInitSize;
	Anchors m_anchors;
	AnchorDists m_initialAnchorDists;
};


class DLL CGuiElementList
{
	friend class CGuiElement;

private:
	typedef std::map<std::string, CGuiElement*, LessThanCppStringObj> GuiList;

public:
	inline CGuiElement* FindElement( const std::string &name ) const {
		GuiList::const_iterator it = m_Map.find(name);
		if( it == m_Map.end() )
			return NULL;
		return it->second;
	}

private:
	inline void DeleteElement( const CGuiElement *pElement ) {
		GuiList::const_iterator it = m_Map.find(pElement->GetName());
		if( it == m_Map.end() )
			return;
		m_Map.erase(it);
	}

	inline void AddElement( CGuiElement *pElement ) {
		m_Map[pElement->GetName()] = pElement;
	}

private:
	GuiList m_Map;
};

//TODO: INTERFACE
extern CGuiElementList *g_pGuiElementList;

POP_PRAGMA_WARNINGS

#define DECLARE_GUI_DATADESC(BaseClassName,ClassName) public: \
	void VParseAdditionalVars( CScriptSubGroup *vars ); \
	DECLARE_CLASS(BaseClassName,ClassName);

#define DATADESC_GUI_BEGIN(classname) \
	void classname::VParseAdditionalVars( CScriptSubGroup *vars ) { \
	BaseClass::VParseAdditionalVars(vars); 

#define DATADESC_GUI_END() }

#define DATA_GUI_VECTOR2(data, name) data = vars->GetVector2d(name, data);
#define DATA_GUI_VECTOR3(data, name) data = vars->GetVector3f(name, data);
#define DATA_GUI_VECTOR4(data, name) data = vars->GetVector4d(name, data);

#define DATA_GUI_COLOR(data, name) data = vars->GetColor(name, data);

#define DATA_GUI_FLOAT(data, name)				data = vars->GetFloat(name, data);
#define DATA_GUI_FLOAT_DEFAULT(data, def, name) data = vars->GetFloat(name, def);
#define DATA_GUI_FLOAT_ARRAY(data, name, num)	vars->GetFloats(name, num, data);
#define DATA_GUI_FLOAT4(data, name)				vars->GetFloats(name, data);

#define DATA_GUI_INT(data, name)				data = vars->GetInt(name, data);
#define DATA_GUI_INT_DEFAULT(data, def, name)	data = vars->GetInt(name, def);
#define DATA_GUI_INT_ARRAY(data, name, num )	vars->GetInts(name, num, data);

#define DATA_GUI_CHAR_ARRAY(data, name, num)	vars->GetString(name, data,num);
#define DATA_GUI_STRING(data, name)				vars->GetString(name, data);


}; //namespace gui

#endif

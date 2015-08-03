// renderer/CGuiElement.cpp
// renderer/CGuiElement.cpp
// renderer/CGuiElement.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CGuiFactory.h"
#include "glheaders.h"
#include "glfunctions.h"
#include <util/debug.h>
#include "renderer.h"

namespace gui
{

CGuiElementList g_GuiElementListObj;
CGuiElementList *g_pGuiElementList = &g_GuiElementListObj;

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CGuiElement::CGuiElement() : m_vPos(vec3_null), m_vSize(0.0f, 0.0f)
{
	m_pParent = NULL;
	g_pGuiElementList->AddElement(this);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CGuiElement::~CGuiElement()
{
	for( size_t i = 0; i < GetNumberOfChildren(); i++ )
	{
		if( m_Children[i] )
			delete m_Children[i];
	}
	m_Children.clear();
	g_pGuiElementList->DeleteElement(this);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::AddChild( CGuiElement *pChild ) 
{ 
	Assert(pChild != this);
	m_Children.push_back(pChild);
	pChild->SetParent( this );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::RemoveChild( CGuiElement *pChild )
{
	int index = FindChild(pChild);
	if( index != -1 )
		RemoveChild( index );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::RemoveChild( size_t index )
{
	ASSERT_CHILD_VECTOR(index);
	m_Children.erase( m_Children.begin() + index );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::RemoveAndDeleteChild( CGuiElement *pChild )
{
	int index = FindChild( pChild );
	if( index != -1 )
		RemoveAndDeleteChild(index);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::RemoveAndDeleteChild( size_t index )
{
	ASSERT_CHILD_VECTOR(index);
	delete m_Children[index];
	m_Children.erase( m_Children.begin() + index );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CGuiElement::FindChild( CGuiElement *pChild )
{
	size_t size = m_Children.size();
	for( size_t i = 0; i < size; i++ )
	{
		if( pChild == m_Children[i] )
			return (int)i;
	}

	return -1;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CGuiElement::IsChild( CGuiElement *pChild )
{
	return FindChild(pChild) != -1;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::Render( void )
{
	if(!IsVisible())
		return;

	bool bPosValid = m_vPos != vec3_null;
	if( bPosValid )
	{
		GetMVStack().PushMatrix();
		GetMVStack().Translate( GetPosition() );
	}

	VRenderBackground();
	VRenderCustom();

	//Render children
	size_t childsize = GetNumberOfChildren();
	for( size_t i = 0; i < childsize; i++ )
		m_Children[i]->Render();

	if( bPosValid )
		GetMVStack().PopMatrix();
}


//---------------------------------------------------------------
// Purpose: All real drawing code goes here!
//---------------------------------------------------------------
void CGuiElement::VRenderCustom( void )
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::DrawQuad( float x1, float y1, float x2, float y2 )
{
	glBindTexture( GL_TEXTURE_2D, Debug::g_iWhiteTexture );
	draw_quad_textured( x1, y1, x2, y2, 0.0f );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::DrawTriangle( const Vector2f &v1, const Vector2f &v2, const Vector2f &v3 )
{
	throw std::exception();
	//glBindTexture( GL_TEXTURE_2D, Debug::g_iWhiteTexture );
	//float vertices[] = {v1.x, v1.y, 0.0f,
	//					v2.x, v2.y, 0.0f,
	//					v3.x, v3.y, 0.0f};
	//GetGLStateSaver()->EnableClientState( GL_VERTEX_ARRAY );
	//glVertexPointer( 3, GL_FLOAT, 0, vertices );
	//glDrawArrays( GL_TRIANGLES, 0, 9 );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::SetDrawColor( float r, float g, float b, float a )
{
	set_draw_color(r,g,b,a);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::SetDrawColor( BYTE r, BYTE g, BYTE b, BYTE a )
{
	set_draw_color((float)r*(1.0f/255.0f), (float)g*(1.0f/255.0f), (float)b*(1.0f/255.0f), (float)a*(1.0f/255.0f) );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::SetDrawColor( Color c )
{
	SetDrawColor(c.r, c.g, c.b, c.a);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::UpdatePosition(bool recursive /* = true */)
{
	Vector2f parentSize = GetParentSize();

	const AnchorDists &init = GetInitialAnchorDists();

	float left, top, right, bottom;
	left = m_anchors.left == ANCHOR_LEFT ? init.left : parentSize.x - init.left;
	top = m_anchors.top == ANCHOR_TOP ? init.top : parentSize.y - init.top;
	right =	m_anchors.right == ANCHOR_LEFT ? init.right : parentSize.x - init.right;
	bottom = m_anchors.bottom == ANCHOR_TOP ? init.bottom : parentSize.y - init.bottom;
	
	m_vPos.x = left;
	m_vPos.y = top;
	m_vSize.x = right - left;
	m_vSize.y = bottom - top;

	if(recursive)
		for(CGuiElement *child : m_Children)
			child->UpdatePosition(true);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::SetAnchors( const Anchors &anchors, bool updatePosition /*= true*/ )
{
	m_anchors = anchors;
	if(updatePosition)
	{
		UpdateAnchorDists();
		UpdatePosition();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::SetInitPos( const Vector3f &pos )
{
	m_vInitPos = pos;
	UpdateAnchorDists();
	UpdatePosition();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::SetInitSize( float x, float y )
{
	m_vInitSize.x = x;
	m_vInitSize.y = y;
	UpdateAnchorDists();
	UpdatePosition();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiElement::UpdateAnchorDists()
{
	Vector2f parentSize = GetParentSize();

	float posLeft = m_vInitPos.x;
	float posTop = m_vInitPos.y;
	float posRight = m_vInitPos.x + m_vInitSize.x;
	float posBottom = m_vInitPos.y + m_vInitSize.y;

	m_initialAnchorDists.left = m_anchors.left == ANCHOR_LEFT ? posLeft : parentSize.x - posLeft;
	m_initialAnchorDists.top = m_anchors.top == ANCHOR_TOP ? posTop : parentSize.y - posTop;
	m_initialAnchorDists.right = m_anchors.right == ANCHOR_LEFT ? posRight : parentSize.x - posRight;
	m_initialAnchorDists.bottom = m_anchors.bottom == ANCHOR_TOP ? posBottom : parentSize.y - posBottom;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector2f CGuiElement::GetParentSize()
{
	CGuiElement *parent = GetParent();
	if(parent)
	{
		return parent->GetSize();
	}
	else
	{
		return Vector2f((float)g_RenderInterf->GetRenderer()->GetScreenWidth(),
			(float)g_RenderInterf->GetRenderer()->GetScreenHeight());
	}
}

} //namespace gui
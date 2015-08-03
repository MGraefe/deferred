// editor/CEditorEntity.cpp
// editor/CEditorEntity.cpp
// editor/CEditorEntity.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"

#include "CEditorEntity.h"
#include "IEditorEntityFactory.h"
#include "OutputWnd.h"
#include <util/collision.h>
#include "CEntityRegister.h"
#include <util/StringUtils.h>
#include "editorDoc.h"
#include <util/debug.h>

DECLARE_EDITOR_ENTITY_TYPE("LOGIC", CEditorEntity);

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntity::CEditorEntity( const CScriptSubGroup *pGrp, int index ) :
	m_index(index)
{
	m_sprite = NULL;
	Deserialize(pGrp);
	InitTranslatableRotatable();
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntity::CEditorEntity( const std::string &classname, int index ) :
	m_classname(classname),
	m_index(index)
{
	AddDefaultParams();
	m_sprite = NULL;
	InitTranslatableRotatable();
}

CEditorEntity::CEditorEntity( const CEditorEntity &other, int index ) :
    m_center(other.m_center),
    m_angles(other.m_angles),
    m_index(index),
    m_keyvals(other.m_keyvals),
    m_outputs(other.m_outputs),
    m_classname(other.m_classname),
	m_spriteName(), //gets parsed in OnCreate
	m_sprite(NULL), //get initialized on OnCreate
    m_bTranslatable(other.m_bTranslatable),
    m_bRotatable(other.m_bRotatable),
    m_zVal(other.m_zVal)
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntity::~CEditorEntity()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntity::Serialize( outstr &str ) const
{
	str << "\"" << GetSerializationHeaderString().c_str() << "\"\n{\n";
	str << "\t\"classname\" \"" << m_classname << "\"\n";
	
	//keyvals
	if( m_keyvals.size() > 0  )
	{
		str << "\t\"keyvals\"\n\t{\n";
		for( auto it = m_keyvals.begin(); it != m_keyvals.end(); it++ )
			str << "\t\t\"" << it->first << "\" \"" << it->second << "\"\n";
		str << "\t}\n\n";
	}

	//outputs
	if( m_outputs.size() > 0 )
	{
		str << "\t\"outputs\"\n\t{\n";
		for(const CEntityOutput *output : m_outputs)
			output->serialize(str, 2);
		str << "\t}\n";
	}

	str << "}\n\n";
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::string CEditorEntity::GetSerializationHeaderString( void ) const
{
	return string("ENTITY");
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CEditorEntity::Deserialize( const CScriptSubGroup *pGrp )
{
	if( !pGrp->GetString( "classname", m_classname ) )
	{
		PostOutput( L"PARSE ERROR: could not find entity classname" );
		return false;
	}

	//When the classname is clear, we can add the default parameters,
	//those might be overwritten later
	AddDefaultParams();

	CScriptSubGroup *grpKeyVals = pGrp->GetSubGroup("keyvals");
	if( !grpKeyVals ) 
	{
		PostOutput( L"PARSE ERROR: could not find keyvals" );
		return false;
	}

	m_center = grpKeyVals->GetVector3f( "origin" );
	m_angles = grpKeyVals->GetVector3f( "angles" );


	const KeyValueMap *pMap = grpKeyVals->GetKeyValueMap();
	KeyValueMap::const_iterator it = pMap->begin();
	KeyValueMap::const_iterator itEnd = pMap->end();

	bool classnameReached = false;
	for( ; it != itEnd; it++ )
	{
		if( !classnameReached && it->first.compare("classname") == 0 )
		{
			classnameReached = true;
			continue;
		}

		m_keyvals[it->first] = it->second;
	}


	//Parse outputs
	CScriptSubGroup *grpOutputs = pGrp->GetSubGroup("outputs");
	if( grpOutputs )
		for(auto it = grpOutputs->GetSubGroupMap()->begin(); it != grpOutputs->GetSubGroupMap()->end(); ++it)
			if(it->first == "output")
				m_outputs.push_back(new CEntityOutput(it->second));

	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::string CEditorEntity::GetName( void ) const
{
	auto it = m_keyvals.find("name");
	if( it == m_keyvals.end() )
		return "";
	else
		return it->second;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CEditorEntity::TraceLine( tracelineinfo_t &info )
{
	//return RaySphereIntersect<float>( m_center, 2.0, info.start, info.dir, info.distMax, info.distHit, false );
	CRay ray(info.start, info.dir);
	AABoundingBox aabb(m_center-2.0f, m_center+2.0f);
	return ray.intersectAABB(aabb, info.distHit, info.distMax);
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntity::InitTranslatableRotatable(void)
{
	//Defaults
	m_bRotatable = false;
	m_bTranslatable = false;

	CEntityRegister *reg = GetEntityRegister();
	Assert(reg);
	CEntityRegisterEntity *regEnt = reg->GetEntity(GetClassnameA());
	if(!regEnt)
		error("Error getting entity register for classname %s", GetClassnameA().c_str());
	else
	{
		m_bTranslatable = regEnt->Get("origin") != NULL;
		m_bRotatable = regEnt->Get("angles") != NULL;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::string CEditorEntity::GetValue( const std::string &key )
{
	auto it = m_keyvals.find( key );

	if( it == m_keyvals.end() ) //Value not found in keyval list? Get Default Value from entity Registry!
	{
		//Lookup default value and insert it
		CEntityRegisterEntity *pDefEnt = GetEntityRegister()->GetEntity( m_classname );
		if( !pDefEnt )
		{
			PostOutput( _T("Could not find values for entity %S"), m_classname.c_str() );
			return m_keyvals[key]; //Constructs empty string entry
		}
		
		CEntityKeyValue *pVal = pDefEnt->Get( key );
		if( !pVal )
		{
			PostOutput( _T("Could not find default value \"%S\" for entity %S"), key.c_str(), m_classname.c_str() );
			return m_keyvals[key]; //Constructs empty string entry
		}

		return m_keyvals[key] = pVal->val;
	}
	

	return it->second;
}


//---------------------------------------------------------------
// Purpose: Do not call from OnParamChanged because of resulting recursion.
//---------------------------------------------------------------
void CEditorEntity::SetValue( const std::string &key, const std::string &val )
{
	std::string old = m_keyvals[key];
	m_keyvals[key] = val;
	OnParamChanged( key, old, val );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CEditorEntity::HasValue( const std::string &key )
{
	return m_keyvals.find(key) != m_keyvals.end();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntity::OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval )
{
	GetActiveDocument()->SetModified();

	if( m_bTranslatable && key.compare("origin") == 0 )
	{
		m_center = StrUtils::GetVector3(newval);
		if( m_sprite )
			m_sprite->SetAbsPos(m_center);
	}
	else if( m_bRotatable && key.compare("angles") == 0 )
	{
		m_angles = StrUtils::GetVector3(newval);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntity::SetSpriteTexture( const std::string &tex )
{
	m_spriteName = tex;
	if( tex.length() > 0 ) 
	{	
		m_sprite = new CSprite(-1,true);
		m_sprite->Init( (std::string("editor\\") + tex).c_str(), m_center, 4.0f, 4.0f );
	}
	else
	{
		if( m_sprite )
			delete m_sprite;
		m_sprite = NULL;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntity::OnCreate( void )
{
	CEntityRegisterEntity *pRegEnt = GetEntityRegister()->GetEntity( m_classname );
	std::string spritename = pRegEnt->GetBaseVal("spritename");
	if( spritename.length() > 0 )
		SetSpriteTexture(spritename);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntity::OnDestroy( void )
{
	if( m_sprite )
		delete m_sprite;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntity::Render( void )
{
	CRenderParams params;
	params.renderpass = RENDERPASS_NORMAL;

	if( IsSelected() )
	{
		GetShaderManager()->GetActiveShader()->EnableTexturing(false);
		//glBindTexture( GL_TEXTURE_2D, Debug::g_iWhiteTexture );
		set_draw_color( 1.0f, 1.0f, 0.0f, 1.0f );
		draw_line_cube( GetAbsCenter(), 4.0f );
		set_draw_color( 1.0f, 1.0f, 1.0f, 1.0f );
	}

	if( m_sprite ) 
	{
		GetShaderManager()->GetActiveShader()->EnableTexturing(true);
		m_sprite->VRender(params);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CEditorEntity::IsSelected( void )
{
	CeditorDoc *pDoc = GetActiveDocument();
	if( pDoc )
		return pDoc->IsSelected(this);

	return false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntity::SetAbsCenter( const Vector3f &center )
{
	if( center == m_center )
		return;

	char vec[128];
	sprintf_s( vec, "%f %f %f", center.x, center.y, center.z );
	SetValue( "origin", vec );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntity::SetAbsAngles( const Angle3d &angles )
{
	if( angles == m_angles )
		return;

	char ang[128];
	sprintf_s( ang, "%f %f %f", angles.x, angles.y, angles.z );
	SetValue( "angles", ang );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CEditorEntity::IsTranslatable( void ) const
{
	return m_bTranslatable;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CEditorEntity::IsRotatable( void ) const
{
	return m_bRotatable;
}


//---------------------------------------------------------------
// Purpose: Add the parameters from our registry which aren't in here
//---------------------------------------------------------------
void CEditorEntity::AddDefaultParams(void)
{
	//Get entity register
	CEntityRegister *reg = GetEntityRegister();
	Assert(reg);
	CEntityRegisterEntity *regEnt = reg->GetEntity(GetClassnameA());
	if(!regEnt)
	{
		error("Error resolving CEntityRegisterEntity for classname %s", GetClassnameA().c_str());
		return;
	}

	for(int i = 0; i < regEnt->size(); i++)
	{
		const CEntityKeyValue *regKv = regEnt->Get(i);
		Assert(regKv);
		if( !HasValue(regKv->name) )
			m_keyvals[regKv->name] = regKv->val;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntity::RemoveOutput( const CEntityOutput *output )
{
	for(int i = 0; i < (int)m_outputs.size(); i++)
	{
		if( m_outputs[i] == output )
		{
			m_outputs.erase(m_outputs.begin() + i);
			i--;
		}
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntity::RemoveOutput( const CEntityOutput &output )
{
	for(int i = 0; i < (int)m_outputs.size(); i++)
	{
		if( *m_outputs[i] == output )
		{
			m_outputs.erase(m_outputs.begin() + i);
			i--;
		}
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntity::UpdateZVal(const Vector3f &camPos, const Vector3f &camDir)
{
	Vector3f camToEnt = GetAbsCenter() - camPos;
	m_zVal = camToEnt.Dot(camDir);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntity::SetOutputList( const OutputList &outputs )
{
	m_outputs = outputs;
}


// editor/CToolRotateEffector.cpp
// editor/CToolRotateEffector.cpp
// editor/CToolRotateEffector.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "CToolRotateEffector.h"
#include "editorDoc.h"
#include "CEditorEntity.h"


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CToolRotateEffector::CToolRotateEffector( const Vector3f &color )
	: CToolEffector(color)
{
	m_scale = 1.0f;
	m_selected = false;
	m_dragging = false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CToolRotateEffector::~CToolRotateEffector()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolRotateEffector::Init( void )
{
	float fsegments = (float)(circle_segments-1);
	for( int i = 0; i < circle_segments; i++ ) 
	{
		float a = ((float)i)/fsegments * (2.0f * PI);
		m_axis_circle[i].Init(0.0f, sin(a), cos(a));
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolRotateEffector::Render( void )
{
	CShader *pShd = GetShaderManager()->GetActiveShader();

	float r = m_baseColor.x;
	float g = m_baseColor.y;
	float b = m_baseColor.z;
	float alphagreater = m_selected ? 0.15f : 0.075f;
	float alphaless = m_selected ? 1.0f : 0.6f;

	if( b > 0.5f ) //boost blue a bit
	{
		alphaless *= 1.5f;
		alphagreater *= 1.5f;
	}

	GetMVStack().PushMatrix();
	GetMVStack().Translate(GetPosition());
	GetMVStack().Rotate(GetLocalRotation().GetInverse());
	GetMVStack().Scale(m_scale, m_scale, m_scale);
	
	GetGLStateSaver()->DepthFunc(GL_GREATER);
	pShd->SetDrawColor(r, g, b, alphagreater);
	draw_line_strip(m_axis_circle, circle_segments);

	GetGLStateSaver()->DepthFunc(GL_LESS);
	pShd->SetDrawColor(r, g, b, alphaless);
	draw_line_strip(m_axis_circle, circle_segments);

	GetMVStack().PopMatrix();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CToolRotateEffector::TraceLine( tracelineinfo_t &info )
{
	//Get the direction relative to our axis
	Quaternion qi = Quaternion(Vector3f(0.0f, 0.0f, 1.0f), 90.0f) * GetLocalRotation();

	Vector3f orig = qi * (info.start - m_position);
	Vector3f dir = qi * info.dir;

	//							in		 in		in	   in		in			out
	if( RaySphereIntersect(vec3_null, m_scale, orig, dir, info.distMax, info.distHit) && info.distHit > 0.0f )
	{
		ConsoleMessage("Intersecting sphere on axis %f %f %f", m_axis.x, m_axis.y, m_axis.z);

		//Get Intersect Point
		Vector3f intersect = orig + dir * info.distHit;
		
		//Rotate the Intersect Point to our sphere's rotation
		//intersect = m_rotation * intersect;

		//Get Sphere coordinates
		float r, phi, theta;
		PointToSphereCoords(intersect, r, phi, theta);

		const float tol = DEG2RAD(3.0f); //tolerance

		//if( m_axis == AXIS_X )
			//return abs(phi) > PI*0.5f-tol && abs(phi) < PI*0.5f+tol;
		//if( m_axis == AXIS_Y )
			return theta > PI*0.5f-tol && theta < PI*0.5f+tol;
		//if( m_axis == AXIS_Z )
		//	return abs(phi) < tol || abs(phi) > PI-tol;
	}

	return false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolRotateEffector::SetPosition( const Vector3f &pos )
{
	m_position = pos;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolRotateEffector::UpdateSize( float cameraDistance, float camFov )
{
	float fTan = TAN(camFov*0.5f);
	m_scale = fTan * cameraDistance * 0.35f;
}


//---------------------------------------------------------------
// Purpose: Approx.
//---------------------------------------------------------------
Vector3f CToolRotateEffector::FindNearestPoint( const Vector3f &rayorig, const Vector3f &raydir )
{
	//Transform the ray into our local system
	Quaternion qi(GetLocalRotation().GetInverse());
	CRay localRay(	qi * (rayorig - m_position),
					qi * raydir );

#if 1
	Vector3f intersPoint;
	CPlane plane(vec3_null, Vector3f(1,0,0));
	
	if( localRay.intersect(plane, intersPoint) )
		return intersPoint.Normalize();
	else
		return vec3_null;
#endif

#if 0 //OLD IMPLEMENTATION
	Vector3f rayPoint;
	float dist = localRay.distToPoint(vec3_null, rayPoint);
	
	//project point on ray onto yz-plane
	rayPoint.x = 0.0f;

	//return normalized
	return rayPoint.GetNormalized();
#endif

#if 0 //OLDER IMPLEMENTATION
	//Do not care about the vertexes behind the front ones if we look at a very sharp angle on it
	float angleToCircle = abs(GetRotateAxis().Dot(raydir));
	bool discardBehind = angleToCircle < COS(80.0f);

	float min = FLOAT_MAX;
	int minIndex = 0;

	for( int i = 0; i < circle_segments; i++ ) 
	{
		if( discardBehind && m_axis_circle[i].Dot(localRay.dir) > 0.0f )
			continue;
		float d = localRay.distToPoint(m_axis_circle[i]);
		if( d < min )
		{
			min = d;
			minIndex = i;
		}
	}

	return m_axis_circle[minIndex];
#endif
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float CToolRotateEffector::GetClickedRotation( const Vector3f &startPointUntransformed, const CRay &clickRay )
{
	Vector3f rotationAxis = GetRotateAxis();

	//Find the new nearest value
	Vector3f nearestPointUntransformed = FindNearestPoint(clickRay.orig, clickRay.dir);

	if( startPointUntransformed == nearestPointUntransformed )
		return 0.0f;
	
	float deg = RAD2DEG(acos(clamp(startPointUntransformed.Dot(nearestPointUntransformed), -1.0f, 1.0f)));
	//Assert(IsFinite(deg));
	if( !IsFinite(deg) )
		return 0.0f;

	if( startPointUntransformed.Cross(nearestPointUntransformed).Dot(Vector3f(1.0f, 0.0f, 0.0f)) > 0.0f )
		return -deg;
	else
		return deg;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolRotateEffector::DragStart( const CRay &clickRay, bool active )
{
	m_dragStartAxis = m_axis;

	if( active )
	{
		m_dragging = true;
		m_dragStartUntransformed = FindNearestPoint(clickRay.orig, clickRay.dir);
		m_rotation = Quaternion();

		m_entityStartPositions.clear();
		m_entityStartRotations.clear();

		const SelectedEntityList &ents = GetActiveDocument()->GetSelectedEntityList();
		for( SelectedEntityList::const_iterator it = ents.begin(); it != ents.end(); ++it )
		{
			m_entityStartPositions.push_back((*it)->GetAbsCenter());
			m_entityStartRotations.push_back((*it)->GetAbsAngles());
		}
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolRotateEffector::DragUpdate( const CRay &clickRay, bool active )
{
	if( !active )
		return;

	float rot = GetClickedRotation(m_dragStartUntransformed, clickRay);
	if( rot == 0.0f )
		return;

	Quaternion newRot(GetRotateAxis(), rot);

	const SelectedEntityList &ents = GetActiveDocument()->GetSelectedEntityList();
	for( size_t i = 0; i < ents.size(); i++ )
	{
		CEditorEntity *pEnt = ents[i];
		Vector3f entStartPos = m_entityStartPositions[i];
		Angle3d entStartRot = m_entityStartRotations[i];
		
		Quaternion entNewRot = newRot * Quaternion(entStartRot);
		Vector3f entNewPos = m_position + newRot * (entStartPos-m_position);
		
		pEnt->SetAbsCenter(entNewPos);
		pEnt->SetAbsAngles(entNewRot.ToAngle());
	}

	m_rotation = newRot;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector3f CToolRotateEffector::GetRotateAxis( void )
{
	return m_axis;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolRotateEffector::DragEnd( bool active )
{
	m_dragging = false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Quaternion CToolRotateEffector::GetLocalRotation()
{
	//Get rotation relative to the x-Axis
	Vector3f xAxis(1.0f, 0.0f, 0.0f);

	if( m_axis == xAxis )
		return Quaternion();

	Vector3f rotAxis = m_axis.Cross(xAxis).Normalize();
	float rotAngle = ACOS(m_axis.Dot(xAxis));
	
	return Quaternion(rotAxis, rotAngle);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const Vector3f & CToolRotateEffector::GetPosition( void )
{
	return m_position;
}



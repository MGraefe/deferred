// util/CPhysConvexCompoundShape.cpp
// util/CPhysConvexCompoundShape.cpp
// util/CPhysConvexCompoundShape.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "CPhysConvexCompoundShape.h"
#include "CPhysConvexHull.h"
#include "StringUtils.h"

const int CPhysConvexCompoundShape::max_shapes(100000);


CPhysConvexCompoundShape::CPhysConvexCompoundShape()
{
	m_aabbValid = false;
	m_materialIndexCounter = 0;
}

CPhysConvexCompoundShape::~CPhysConvexCompoundShape()
{
	for(CPhysConvexHull *shape : m_shapes)
	{
		if(shape)
		{
			delete shape;
			shape = NULL;
		}
	}
}


void CPhysConvexCompoundShape::AddShape(CPhysConvexHull *shape, const std::string &material)
{
	int index;

	//material already there?
	auto fIt = m_materialIndexMap.find(material);
	if( fIt != m_materialIndexMap.end() )
		index = fIt->second;
	else
		index = m_materialIndexMap[material] = ++m_materialIndexCounter;
	shape->SetMaterialIndex(index);
	m_shapes.push_back(shape);
}


//serialize to an ostream
error_e CPhysConvexCompoundShape::Serialize(std::ostream &os) const
{
	UINT materials = m_materialIndexMap.size();
	WRITE_BINARY(os, materials);
	for(const std::pair<std::string, int> &pair : m_materialIndexMap)
	{
		StrUtils::binarizeString(os, pair.first);
		WRITE_BINARY(os, pair.second);
	}

	WRITE_BINARY(os, m_aabb);
	UINT shapes = m_shapes.size();
	WRITE_BINARY(os, shapes);
	for(CPhysConvexHull *shape : m_shapes)
	{
		error_e ret = shape->Serialize(os);
		if( ret != ERR_NONE )
			return ret;
	}

	return os.good() ? ERR_NONE : ERR_CORRUPT_FILE;
}


error_e CPhysConvexCompoundShape::Deserialize(std::istream &is)
{
	if( !is.good() )
		return ERR_CORRUPT_FILE;

	UINT materials = 0;
	READ_BINARY(is, materials);
	for( UINT i = 0; i < materials; i++ )
	{
		std::pair<std::string, int> pair;
		StrUtils::debinarizeString(is, pair.first);
		READ_BINARY(is, pair.second);
		if( !is.good() )
			return ERR_CORRUPT_FILE;
		m_materialIndexMap.insert(pair);
	}

	UINT shapes = 0;
	READ_BINARY(is, m_aabb);
	READ_BINARY(is, shapes);
	if( shapes > (UINT)max_shapes )
		return ERR_CORRUPT_FILE;
		
	for( UINT i = 0; i < shapes; i++ )
	{
		if( !is.good() )
			return ERR_CORRUPT_FILE;
		CPhysConvexHull *shape = new CPhysConvexHull();
		error_e ret = shape->Deserialize(is);
		if( ret != ERR_NONE )
			return ret;
		m_shapes.push_back(shape);
	}
	
	return is.good() ? ERR_NONE : ERR_CORRUPT_FILE;
}


void CPhysConvexCompoundShape::CalculateAABB(void)
{
	if( GetSize() < 1 )
		return;

	m_aabb.min = m_aabb.max = m_shapes[0]->GetVertex(0);

	for(CPhysConvexHull *shape : m_shapes) //For all shapes
	{
		for(const Vector3f &v : shape->GetVertexList()) //For all vertices of the shape
		{
			m_aabb.min = m_aabb.min.MinComponentWise(v);
			m_aabb.max = m_aabb.max.MaxComponentWise(v);
		}
	}

	m_aabbValid = m_aabb.min != m_aabb.max;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::string CPhysConvexCompoundShape::GetMaterialNameById( int id ) const
{
	auto it = std::find_if(m_materialIndexMap.begin(), m_materialIndexMap.end(),
		[id](MaterialIndexMap::const_reference p){ return p.second == id; });

	if(it != m_materialIndexMap.end())
		return it->first;
	else
		return std::string();
}

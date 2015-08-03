// util/CPhysFile.h
// util/CPhysFile.h
// util/CPhysFile.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__CPhysFile_H__
#define deferred__util__CPhysFile_H__

#include <string>
#include <vector>

class CPhysConvexCompoundShape;
class CPhysFile
{
public:
	static const short int version;
	static const UINT max_shapes;
	static const UINT file_ident;

	CPhysFile();
	~CPhysFile();

	//Serialize to binary-stream
	error_e Serialize(std::ostream &os);

	//Serialize to file
	error_e Serialize(const char *filename);

	//Deserialize from binary-stream
	error_e Deserialize(std::istream &is);

	//Deserialize from file
	error_e Deserialize(const char *filename);

	//At the moment only one shape is supported
	const CPhysConvexCompoundShape *GetShape(void) const { return m_physShapes.empty() ? NULL : m_physShapes[0]; }

	//Shapes get deleted if this object is deleted!
	void AddShape(CPhysConvexCompoundShape *shape) { m_physShapes.push_back(shape); }

	const Vector3f &GetCenterOfMass() const { return centerOfMass; }
	void SetCenterOfMass(const Vector3f &center) { centerOfMass = center; }

	float GetMass() const { return mass; }
	void SetMass(float kilograms) { mass = kilograms; }

private:
	Vector3f centerOfMass;
	float mass;
	std::vector<CPhysConvexCompoundShape*> m_physShapes;
};

#endif
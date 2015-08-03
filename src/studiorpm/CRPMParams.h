// studiorpm/CRPMParams.h
// studiorpm/CRPMParams.h
// studiorpm/CRPMParams.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once
#ifndef deferred__studiorpm__CRPMParams_H__
#define deferred__studiorpm__CRPMParams_H__

#include <string>
#include <util/maths.h>

class CRPMParams
{
public:
	virtual ~CRPMParams() { }
	virtual std::string GetModelInputFile(void) = 0;
	virtual std::wstring GetModelInputFileW(void) = 0;
	virtual std::string GetModelOutputFile(void) = 0;
	virtual std::wstring GetModelOutputFileW(void) = 0;
	virtual Angle3d GetModelRotate(void) = 0;
	virtual float GetModelScale(void) = 0;
	virtual std::string GetModelTexturesGlobalPrefix(void) = 0;
	//Also applies the global prefix
	virtual std::string GetModelTexturesReplaceName(const std::string &smdName) = 0;
	virtual bool IsPhysSectionThere(void) = 0;
	virtual std::string GetPhysInputFile(void) = 0;
	virtual std::wstring GetPhysInputFileW(void) = 0;
	virtual std::string GetPhysOutputFile(void) = 0;
	virtual std::wstring GetPhysOutputFileW(void) = 0;
	virtual float GetPhysMass(void) = 0;
	virtual Vector3f GetPhysCenterOfMass(void) = 0;
	virtual bool GetPhysCompoundShape(void) = 0;
	virtual std::string GetPhysDefaultMaterial(void) = 0;
	virtual std::string GetPhysCompoundShapeMaterial(const std::string &bone) = 0;
	virtual bool GetPhysUseMayaAxisForCOM(void) = 0;
	virtual bool RecalcNormals(void) = 0;
};

#endif // deferred__studiorpm__CRPMParams_H__
// studiorpm/CRPMParamInput.h
// studiorpm/CRPMParamInput.h
// studiorpm/CRPMParamInput.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__studiorpm__CRPMParamInput_H__
#define deferred__studiorpm__CRPMParamInput_H__

#include <util/StringUtils.h>
#include "CRPMParams.h"

using namespace std;


class CRPMParamInput : public CRPMParams
{
public:
	CRPMParamInput(const wstring &input, const wstring &output, const wstring &texprefix = L"", const wstring &scale = L"1.0", const wstring &rotation = L"0 0 0");
	string GetModelInputFile(void);
	wstring GetModelInputFileW(void);
	string GetModelOutputFile(void);
	wstring GetModelOutputFileW(void);
	Angle3d GetModelRotate(void);
	float GetModelScale(void);
	string GetModelTexturesGlobalPrefix(void);
	//Also applies the global prefix
	string GetModelTexturesReplaceName(const string &smdName);
	bool IsPhysSectionThere(void);
	string GetPhysInputFile(void);
	wstring GetPhysInputFileW(void);
	string GetPhysOutputFile(void);
	wstring GetPhysOutputFileW(void);
	float GetPhysMass(void);
	Vector3f GetPhysCenterOfMass(void);
	bool GetPhysCompoundShape(void);
	string GetPhysDefaultMaterial(void);
	string GetPhysCompoundShapeMaterial(const string &bone);
	bool GetPhysUseMayaAxisForCOM(void);
	bool RecalcNormals(void);

private:
	void processInputFilename(wstring &in);
	wstring m_smdFilename;
	wstring m_rpmFilename;
	Angle3d m_rotation;
	float m_scale;
	string m_texPrefix;
};


#endif // deferred__studiorpm__CRPMParamInput_H__
// studiorpm/CRPMScriptParser.h
// studiorpm/CRPMScriptParser.h
// studiorpm/CRPMScriptParser.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__studiorpm__CRPMScriptParser_H__
#define deferred__studiorpm__CRPMScriptParser_H__

#include <util/CScriptParser.h>
#include "CRPMParams.h"

class CRPMScriptParser : public CRPMParams
{
public:
	CRPMScriptParser();
	error_e ParseFile(const char *filename);
	std::string GetModelInputFile(void);
	std::wstring GetModelInputFileW(void);
	std::string GetModelOutputFile(void);
	std::wstring GetModelOutputFileW(void);
	Angle3d GetModelRotate(void);
	float GetModelScale(void);
	std::string GetModelTexturesGlobalPrefix(void);
	//Also applies the global prefix
	std::string GetModelTexturesReplaceName(const std::string &smdName);
	bool IsPhysSectionThere(void);
	std::string GetPhysInputFile(void);
	std::wstring GetPhysInputFileW(void);
	std::string GetPhysOutputFile(void);
	std::wstring GetPhysOutputFileW(void);
	float GetPhysMass(void);
	Vector3f GetPhysCenterOfMass(void);
	bool GetPhysCompoundShape(void);
	std::string GetPhysDefaultMaterial(void);
	std::string GetPhysCompoundShapeMaterial(const std::string &bone);
	bool GetPhysUseMayaAxisForCOM(void);
	bool RecalcNormals(void);

private:
	CScriptSubGroup *GetModelGrp(void);
	CScriptSubGroup *GetModelTexGrp(void);
	CScriptSubGroup *GetPhysGrp(void);

	bool m_fileParsed;
	CScriptParser m_file;
};

#endif // deferred__studiorpm__CRPMScriptParser_H__
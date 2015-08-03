// studiorpm/CRPMParamInput.cpp
// studiorpm/CRPMParamInput.cpp
// studiorpm/CRPMParamInput.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "CRPMParamInput.h"
#include "studiorpm.h"
#include "unicode.h"


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRPMParamInput::processInputFilename( wstring &in )
{
	while(!in.empty() && (in[0] == ' ' || in[0] == '\t' || in[0] == '"' || in[0] == '\''))
		in.erase(0);
	size_t l = in.size()-1;
	while(!in.empty() && (in[l] == ' ' || in[l] == '\t' || in[l] == '"' || in[l] == '\''))
	{
		in.erase(l);
		l--;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRPMParamInput::GetPhysUseMayaAxisForCOM( void )
{
	THROW_INVALID_FUNCTION_CALL();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMParamInput::GetPhysCompoundShapeMaterial( const string &bone )
{
	THROW_INVALID_FUNCTION_CALL();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMParamInput::GetPhysDefaultMaterial( void )
{
	THROW_INVALID_FUNCTION_CALL();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRPMParamInput::GetPhysCompoundShape( void )
{
	THROW_INVALID_FUNCTION_CALL();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector3f CRPMParamInput::GetPhysCenterOfMass( void )
{
	THROW_INVALID_FUNCTION_CALL();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float CRPMParamInput::GetPhysMass( void )
{
	THROW_INVALID_FUNCTION_CALL();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::wstring CRPMParamInput::GetPhysOutputFileW( void )
{
	THROW_INVALID_FUNCTION_CALL();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMParamInput::GetPhysOutputFile( void )
{
	THROW_INVALID_FUNCTION_CALL();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::wstring CRPMParamInput::GetPhysInputFileW( void )
{
	THROW_INVALID_FUNCTION_CALL();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMParamInput::GetPhysInputFile( void )
{
	THROW_INVALID_FUNCTION_CALL();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRPMParamInput::IsPhysSectionThere( void )
{
	return false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMParamInput::GetModelTexturesReplaceName( const string &smdName )
{
	string name = smdName;
	StrUtils::replaceFileExtension(name, "");
	return GetModelTexturesGlobalPrefix() + name;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMParamInput::GetModelTexturesGlobalPrefix( void )
{
	return m_texPrefix;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float CRPMParamInput::GetModelScale( void )
{
	return m_scale;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Angle3d CRPMParamInput::GetModelRotate( void )
{
	return m_rotation;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::wstring CRPMParamInput::GetModelOutputFileW( void )
{
	return m_rpmFilename;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMParamInput::GetModelOutputFile( void )
{
	string ret;
	w2a(m_rpmFilename, ret);
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::wstring CRPMParamInput::GetModelInputFileW( void )
{
	return m_smdFilename;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMParamInput::GetModelInputFile( void )
{
	string ret;
	w2a(m_smdFilename, ret);
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CRPMParamInput::CRPMParamInput( const wstring &input, const wstring &output, const wstring &texprefix /*= L""*/, const wstring &scale /*= L"1.0"*/, const wstring &rotation /*= L"0 0 0"*/ ) :
	m_smdFilename(input),
	m_rpmFilename(output)
{
	processInputFilename(m_smdFilename);
	processInputFilename(m_rpmFilename);

	string aScale;
	w2a(scale, aScale);
	m_scale = StrUtils::GetFloat(aScale);

	string aRotation;
	w2a(rotation, aRotation);
	m_rotation = StrUtils::GetVector3(aRotation);

	w2a(texprefix, m_texPrefix);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRPMParamInput::RecalcNormals( void )
{
	return true;
}

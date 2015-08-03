// studiorpm/CRPMScriptParser.cpp
// studiorpm/CRPMScriptParser.cpp
// studiorpm/CRPMScriptParser.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "studiorpm.h"
#include "CRPMScriptParser.h"
#include "unicode.h"

using namespace std;

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CScriptSubGroup * CRPMScriptParser::GetPhysGrp( void )
{
	return m_file.GetSubGroup("physics");
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CScriptSubGroup * CRPMScriptParser::GetModelTexGrp( void )
{
	return GetModelGrp()->GetSubGroup("textures");
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CScriptSubGroup * CRPMScriptParser::GetModelGrp( void )
{
	return m_file.GetSubGroup("model");
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRPMScriptParser::GetPhysUseMayaAxisForCOM( void )
{
	Assert(IsPhysSectionThere());
	return GetPhysGrp()->GetInt("useMayaAxisForCOM", 1) == 1;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMScriptParser::GetPhysCompoundShapeMaterial( const string &bone )
{
	Assert(IsPhysSectionThere());
	Assert(GetPhysCompoundShape());
	string ret = GetPhysDefaultMaterial();
	CScriptSubGroup *compoundShapesGrp = GetPhysGrp()->GetSubGroup("compoundShapes");
	if( compoundShapesGrp )
	{
		CScriptSubGroup *shapeGrp = compoundShapesGrp->GetSubGroup(bone.c_str());
		if( shapeGrp )
			shapeGrp->GetString("material", ret);
	}

	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMScriptParser::GetPhysDefaultMaterial( void )
{
	Assert(IsPhysSectionThere());
	string ret("metal");
	GetPhysGrp()->GetString("defaultMaterial", ret);
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRPMScriptParser::GetPhysCompoundShape( void )
{
	Assert(IsPhysSectionThere());
	return GetPhysGrp()->GetInt("compoundShape", 1) == 1;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector3f CRPMScriptParser::GetPhysCenterOfMass( void )
{
	Assert(IsPhysSectionThere());
	Vector3f com = GetPhysGrp()->GetVector3f("centerOfMass");
	com *= GetModelScale(); // Default is 0 0 0
	if(GetPhysUseMayaAxisForCOM())
		com = Vector3f(com.x, com.z, -com.y);
	Angle3d modelRotate = GetModelRotate();

	if( modelRotate != vec3_null )
	{
		Matrix3 mat;
		GetAngleMatrix3x3( GetModelRotate(), mat );
		com = mat * com;
	}
	return com;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float CRPMScriptParser::GetPhysMass( void )
{
	Assert(IsPhysSectionThere());
	return GetPhysGrp()->GetFloat("mass", 100.0f);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::wstring CRPMScriptParser::GetPhysOutputFileW( void )
{
	wstring ret;
	a2w(GetPhysOutputFile(), ret);
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMScriptParser::GetPhysOutputFile( void )
{
	return GetModelOutputFile() + ".phys";
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::wstring CRPMScriptParser::GetPhysInputFileW( void )
{
	wstring ret;
	a2w(GetPhysInputFile(), ret);
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMScriptParser::GetPhysInputFile( void )
{
	Assert(IsPhysSectionThere());
	string ret;
	GetPhysGrp()->GetString("inputFile", ret);
	return handleFilenameVariables(ret);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRPMScriptParser::IsPhysSectionThere( void )
{
	return GetPhysGrp() != NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMScriptParser::GetModelTexturesReplaceName( const string &smdName )
{
	string name;
	CScriptSubGroup *replaceTexsGrp = NULL;
	if( !GetModelTexGrp() || 
		!(replaceTexsGrp = GetModelTexGrp()->GetSubGroup("replaceTextures")) ||
		!replaceTexsGrp->GetString(smdName, name) )
	{
		//Build default
		name = smdName;
		StrUtils::replaceFileExtension(name, "");
	}

	return GetModelTexturesGlobalPrefix() + name;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMScriptParser::GetModelTexturesGlobalPrefix( void )
{
	string ret("");
	if( GetModelTexGrp() )
		GetModelTexGrp()->GetString("globalPrefix", ret);
	return handleFilenameVariables(ret);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float CRPMScriptParser::GetModelScale( void )
{
	return GetModelGrp()->GetFloat("scale", 1.0f);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Angle3d CRPMScriptParser::GetModelRotate( void )
{
	return GetModelGrp()->GetVector3f("rotate"); //Default is (0 0 0)
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::wstring CRPMScriptParser::GetModelOutputFileW( void )
{
	wstring ret;
	a2w(GetModelOutputFile(), ret);
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMScriptParser::GetModelOutputFile( void )
{
	string ret;
	if( !GetModelGrp()->GetString("outputFile", ret) )
	{
		ret = GetModelInputFile();
		StrUtils::replaceFileExtension(ret, ".rpm");
	}
	return handleFilenameVariables(ret);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::wstring CRPMScriptParser::GetModelInputFileW( void )
{
	wstring ret;
	a2w(GetModelInputFile(), ret);
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
string CRPMScriptParser::GetModelInputFile( void )
{
	string ret;
	m_file.GetSubGroup("model")->GetString("inputFile", ret);
	return handleFilenameVariables(ret);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
error_e CRPMScriptParser::ParseFile( const char *filename )
{
	if( m_fileParsed )
		return ERR_ALREADY_DONE;
	if( !m_file.ParseFile(filename) )
		return ERR_FILENOTFOUND;

	//Assert this is a valid file with all nessecary components
	CScriptSubGroup *modelGrp = NULL;
	if( !(modelGrp = m_file.GetSubGroup("model")) )
	{
		wcout << "ERROR: Script is missing the \"model\" block." << endl;
		return ERR_CORRUPT_FILE;
	}
	string dummy;
	if( !modelGrp->GetString("inputFile", dummy) )
	{
		wcout << "ERROR: Script is missing the \"inputFile\" in the \"model\" block." << endl;
		return ERR_CORRUPT_FILE;
	}
	if( IsPhysSectionThere() && !GetPhysGrp()->GetString("inputFile", dummy) )
	{
		wcout << "ERROR: physics-block is present, but inside no \"inputFile\"." << endl;
		return ERR_CORRUPT_FILE;
	}

	m_fileParsed = true;

	return ERR_NONE;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CRPMScriptParser::CRPMScriptParser()
{
	m_fileParsed = false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRPMScriptParser::RecalcNormals( void )
{
	return GetModelGrp()->GetInt("recalcNormals", 1) == 1;
}

// util/datadesc.cpp
// util/datadesc.cpp
// util/datadesc.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "datadesc.h"

using std::string;

CDatadescEntry::CDatadescEntry( datadesc_type_e Type, int Count, size_t DataOfs, int Flags, const char *Varname ) :
	type(Type),
	count(Count),
	dataOfs(DataOfs),
	flags(Flags),
	varname(Varname)
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
#define GET_DATA_PTR(type) (reinterpret_cast<type*>(reinterpret_cast<size_t>(dataPtr) + dataOfs))
bool CDatadescEntry::ReadFromScript( CScriptSubGroup *pScript, void *dataPtr )
{
	bool bNoError = true;
	switch( type )
	{
	case DATA_INT:
	case DATA_INT_ARRAY:
		bNoError = pScript->GetInts( varname.c_str(), count, GET_DATA_PTR(int) );
		break;

	case DATA_BOOL:
		*GET_DATA_PTR(bool) = pScript->GetInt( varname.c_str(), 0 ) != 0;
		break;

	case DATA_FLOAT:
	case DATA_FLOAT4:
	case DATA_VECTOR2:
	case DATA_VECTOR3:
	case DATA_VECTOR4:
	case DATA_FLOAT_ARRAY:
		bNoError = pScript->GetFloats( varname.c_str(), count, GET_DATA_PTR(float) );
		break;

	case DATA_VECTOR3_NORMALIZED:
		bNoError = pScript->GetFloats( varname.c_str(), count, GET_DATA_PTR(float) );
		if( bNoError )
			GET_DATA_PTR(Vector3f)->Normalize();
		break;

	case DATA_VECTOR4_NORMALIZED:
		bNoError = pScript->GetFloats( varname.c_str(), count, GET_DATA_PTR(float) );
		if( bNoError )
			GET_DATA_PTR(Vector4f)->Normalize();
		break;

	case DATA_COLOR3:
	case DATA_COLOR4:
		{
			bNoError = pScript->GetFloats( varname.c_str(), count, GET_DATA_PTR(float) );
			if( bNoError )
			{
				GET_DATA_PTR(Vector3f)->x /= 255.0f;
				GET_DATA_PTR(Vector3f)->y /= 255.0f;
				GET_DATA_PTR(Vector3f)->z /= 255.0f;
			}
			break;
		}

	case DATA_STRING:
		bNoError = pScript->GetString( varname, *GET_DATA_PTR(string) );
		break;

	case DATA_CHAR_ARRAY:
		bNoError = pScript->GetString( varname.c_str(), GET_DATA_PTR(char), count );
		break;

	case DATA_ANGLES_TO_QUATERNION:
		{
			Angle3d ang;
			bNoError = pScript->GetFloats( varname.c_str(), count, (float*)&ang );
			if( bNoError )
				GET_DATA_PTR(Quaternion)->SetFromAngle(ang);
			break;
		}

	default:
		error("CDatadescEntry::ReadFromScript(): Unrecognized datadesc_type_e");
		return false;
	}

	if( !bNoError )
		ConsoleMessage( "CDatadescEntry::ReadFromScript(): Value not found: %s", varname.c_str() );

	return bNoError;
}


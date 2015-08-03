// studiorpm/studiorpm.cpp
// studiorpm/studiorpm.cpp
// studiorpm/studiorpm.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
// Studiorpm:
// Converts Valve SMD (http://developer.valvesoftware.com/wiki/SMD) into RedPuma Model (RPM).
// Copyright 2009 Marius Gräfe.
//

#include "studiorpm.h"
#include "CBone.h"
#include "smdfile.h"
#include "CObjInputFile.h"
#include "CRPMParams.h"
#include "CRPMParamInput.h"
#include "CRPMScriptParser.h"
#include "unicode.h"

using namespace std;




#define QUIT /*fflush(stdout);*/ /*system("PAUSE");*/ return 0
#define ULONG unsigned long

float starttime;
LARGE_INTEGER iFrequency;

void StartTimer( void )
{
	LARGE_INTEGER iTime;
	QueryPerformanceFrequency( &iFrequency );
	QueryPerformanceCounter( &iTime );

	starttime = (float)((double)iTime.QuadPart / (double)iFrequency.QuadPart);
}
float StopTimer( void )
{
	LARGE_INTEGER iTime;
	QueryPerformanceCounter( &iTime );
	return (float)( ( (double)iTime.QuadPart / (double)iFrequency.QuadPart ) - starttime );
}

//bool opensmd( char *chSMDFile, int FileLength, rpm_file_t *rpmfile );
bool PointEqual( Vector3f a, Vector3f b, float tolerance = 0 )
{
	if( a[0] >= b[0] - tolerance &&
		a[0] <= b[0] + tolerance &&
		a[1] >= b[1] - tolerance &&
		a[1] <= b[1] + tolerance &&
		a[2] >= b[2] - tolerance &&
		a[2] <= b[2] + tolerance )
		return true;
	else
		return false;
}


void ComputeVertexNormal( rpm_file_t *file, int v )
{
	///////////////////////////////////
	//Calculate Vertex normals out of Triangle normal!
	///////////////////////////////////
	rpm_triangle_t * tris = (rpm_triangle_t*)file->findLump(LUMP_TRIANGLES)->getData();
	int iTri = v / 3;
	int iVer = v % 3;

	Vector3f vSum = Vector3f( 0.0f, 0.0f, 0.0f );
	int iAverageDiv = 0;
	for( UINT i = 0; i < file->header.num_of_triangles; i++ )
	{
		if( PointEqual( tris[i].vertex3[0].pos3, tris[iTri].vertex3[iVer].pos3, 0.000001f ) )
		{
			vSum += Vector3f( tris[i].norm3[0], tris[i].norm3[1], tris[i].norm3[2] );
			iAverageDiv++;
		}
		else if( PointEqual( tris[i].vertex3[1].pos3, tris[iTri].vertex3[iVer].pos3, 0.000001f ) )
		{
			vSum += Vector3f( tris[i].norm3[0], tris[i].norm3[1], tris[i].norm3[2] );
			iAverageDiv++;
		}
		else if( PointEqual( tris[i].vertex3[2].pos3, tris[iTri].vertex3[iVer].pos3, 0.000001f ) )
		{
			vSum += Vector3f( tris[i].norm3[0], tris[i].norm3[1], tris[i].norm3[2] );
			iAverageDiv++;
		}
	}
	//All intersection with this vertex clear, calculate the normal!
	Vector3f vVertNorm = vSum / (float)iAverageDiv;
	VectorNormalize( vVertNorm );
	//store
	tris[iTri].vertex3[iVer].norm3 = vVertNorm;
}


CInputFile *createFileObject(const std::string &filename)
{
	size_t lastDot = filename.find_last_of('.');
	if(lastDot == string::npos)
	{
		wcout << "No file extension!\n";
		QUIT;
	}
	string fileExt(filename, lastDot+1);

	CInputFile *inputFile = NULL;
	if(fileExt == "smd")
		inputFile = new CSMDInputFile();
	else if(fileExt == "obj")
		inputFile = new CObjInputFile();
	else
		wcout << "Unknown file extension \"" << fileExt.c_str() << "\".\n";

	return inputFile;
}


//rotate by "a" because Valve SMD has y-axis up for some reason...
void RotateVertsAroundXAxis( float a, rpm_triangle_t *tris, int numTris )
{
	//y' = y*cos q - z*sin q
	//z' = y*sin q + z*cos q
	float y,z;
	float sa = SIN(a);
	float ca = COS(a);

	for( int i = 0; i < numTris; i++ ) //Triangles
	{
		for( int k = 0; k < 3; k++ ) //Vertexes
		{
			//Rotate Vertex
			y = tris[i].vertex3[k].pos3[1];
			z = tris[i].vertex3[k].pos3[2];
			tris[i].vertex3[k].pos3[1] = y*ca - z*sa;
			tris[i].vertex3[k].pos3[2] = y*sa + z*ca;

			//Rotate Normal
			y = tris[i].vertex3[k].norm3[1];
			z = tris[i].vertex3[k].norm3[2];
			tris[i].vertex3[k].norm3[1] = y*ca - z*sa;
			tris[i].vertex3[k].norm3[2] = y*sa + z*ca;
		}
	}
}


void RotateVertsByAngles( const Angle3d &ang, rpm_triangle_t *tris, int numTris )
{
	Matrix3 mat;
	GetAngleMatrix3x3( ang, mat );


	//y' = y*cos q - z*sin q
	//z' = y*sin q + z*cos q
	Vector3f vert, norm;

	for( int i = 0; i < numTris; i++ ) //Triangles
	{
		for( size_t k = 0; k < 3; k++ ) //Vertexes
		{
			//Rotate Vertex
			vert.Init( tris[i].vertex3[k].pos3 );
			vert = mat * vert;
			tris[i].vertex3[k].pos3[0] = vert.x;
			tris[i].vertex3[k].pos3[1] = vert.y;
			tris[i].vertex3[k].pos3[2] = vert.z;

			//Rotate Normal
			norm.Init( tris[i].vertex3[k].norm3 );
			norm = mat * norm;
			tris[i].vertex3[k].norm3[0] = norm.x;
			tris[i].vertex3[k].norm3[1] = norm.y;
			tris[i].vertex3[k].norm3[2] = norm.z;
		}
	}
}


char *CopyUntilOccurenceOfChar( char *out, char *in, char *delimiter )
{
	char *pDelimiter = strstr( in, delimiter );
	int num_of_chars = pDelimiter - in;
	strncpy( out, in, num_of_chars );
	out[num_of_chars] = NULL;
	return pDelimiter+1;
}

float FindMinMaxInVertexes( rpm_file_t *rpmfile, int xyz, bool min )
{
	float erg = 0;
	rpm_triangle_t *tris = (rpm_triangle_t*)rpmfile->findLump(LUMP_TRIANGLES)->getData();
	for( UINT i = 0; i < rpmfile->header.num_of_triangles; i++ )
	{
		if( min )
		{
			for( int k = 0; k < 3; k++ )
				if( tris[i].vertex3[k].pos3[xyz] < erg )
					erg = tris[i].vertex3[k].pos3[xyz];
		}
		else
		{
			for( int k = 0; k < 3; k++ )
				if( tris[i].vertex3[k].pos3[xyz] > erg )
					erg = tris[i].vertex3[k].pos3[xyz];
		}
	}
	return erg;
}


//------------------------------------------------------------------------
//Purpose: Scales the whole Map
//------------------------------------------------------------------------
void ScaleRPM( rpm_file_t &rpm, float factor )
{
	rpm_triangle_t *tris = (rpm_triangle_t*)rpm.findLump(LUMP_TRIANGLES)->getData();
	for( UINT i = 0; i < rpm.header.num_of_triangles; i++ )
		for( int k = 0; k < 3; k++ )
			tris[i].vertex3[k].pos3 *= factor;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void ScaleTriangles( vector<rpm_triangle_t> &verts, float scale )
{
	for( rpm_triangle_t &v : verts )
		for( int i = 0; i < 3; i++ )
			v.vertex3[i].pos3 *= scale;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CalcBoundingBox( rpm_file_t &rpmfile )
{
	wcout<<"Calculating Bounding Box\n";

	//Find minimum of all 3 coords!
	rpmfile.header.bounds.max[0] = FindMinMaxInVertexes( &rpmfile, 0, false );
	rpmfile.header.bounds.max[1] = FindMinMaxInVertexes( &rpmfile, 1, false );
	rpmfile.header.bounds.max[2] = FindMinMaxInVertexes( &rpmfile, 2, false );
	rpmfile.header.bounds.min[0] = FindMinMaxInVertexes( &rpmfile, 0, true );
	rpmfile.header.bounds.min[1] = FindMinMaxInVertexes( &rpmfile, 1, true );
	rpmfile.header.bounds.min[2] = FindMinMaxInVertexes( &rpmfile, 2, true );

	wcout<<"Min. was:\n"<< rpmfile.header.bounds.min[0] << " "
		<< rpmfile.header.bounds.min[1] << " "
		<< rpmfile.header.bounds.min[2] << ".\nMax was:\n"
		<< rpmfile.header.bounds.max[0] << " "
		<< rpmfile.header.bounds.max[1] << " "
		<< rpmfile.header.bounds.max[2] << ".\n";
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void calcTriangleNormal( rpm_triangle_t &tri )
{
	Vector3f v1 = tri.vertex3[1].pos3 - tri.vertex3[0].pos3;
	Vector3f v2 = tri.vertex3[2].pos3 - tri.vertex3[0].pos3;

	tri.norm3 = VectorCross(v1,v2); //ccw = v2 x v1, cw = v1 x v2
	VectorNormalize( tri.norm3 );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void assembleTriangles(vector<rpm_triangle_t> &triangles, rpm_file_t &rpm)
{
	rpm.header.num_of_triangles = triangles.size();
	rpm_triangle_t *tris = new rpm_triangle_t[triangles.size()];
	memcpy(tris, &triangles[0], triangles.size()*sizeof(rpm_triangle_t));
	rpm.addLump(rpm_lump(LUMP_TRIANGLES, triangles.size()*sizeof(rpm_triangle_t), (char*)tris));
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void assembleTextureNames(vector<string> &textures, rpm_file_t &rpm, CRPMParams *params)
{
	int totalLength = 0;
	for( string &texName : textures )
	{
		texName = params->GetModelTexturesReplaceName(texName);
		totalLength += texName.size() + 1;
	}

	//Set rpm values
	rpm.header.iTexdataLength = totalLength;
	rpm.header.sTexOfsetts = textures.size();
	char *texnames = new char[totalLength];
	int *texofs = new int[textures.size()];
	
	//Copy texnames
	char *ptr = texnames;
	for( size_t i = 0; i < textures.size(); i++ )
	{
		texofs[i] = static_cast<int>(ptr - texnames);
		strcpy(ptr, textures[i].c_str());
		ptr += textures[i].length()+1;
	}

	//add lumps
	rpm.addLump(rpm_lump(LUMP_TEXNAMES, totalLength, texnames));
	rpm.addLump(rpm_lump(LUMP_TEXOFS, textures.size() * sizeof(int), (char*)texofs));
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int getTextureIndex( std::vector<std::string> &textures, const char *tex )
{
	for( size_t i = 0; i < textures.size(); ++i )
		if( textures[i].compare(tex) == 0 )
			return (int)i;
	return -1;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
error_e handlePhysics(CRPMParams *params)
{
	if(!params->IsPhysSectionThere())
		return ERR_NONE;

	cout << "Processing physics file: \"" << params->GetPhysInputFile() << "\"" << endl;
	std::unique_ptr<CInputFile> inputFile(createFileObject(params->GetPhysInputFile()));
	if(!inputFile)
		return ERR_CORRUPT_FILE;
	error_e e;
	if( (e = inputFile->parse(params->GetPhysInputFile())) != ERR_NONE )
	{
		std::cout << "ERROR: " << ToString(e) << "\n";
		return e;
	}

	std::vector<rpm_triangle_t> &triangles = inputFile->triangles();
	std::vector<CBone> &boneList = inputFile->bones();
	std::vector<int> &parentBones = inputFile->parentBones();

	ScaleTriangles(triangles, params->GetModelScale());
	RotateVertsAroundXAxis(-90, &triangles[0], triangles.size());
	if(params->GetModelRotate() != vec3_null)
		RotateVertsByAngles(params->GetModelRotate(), &triangles[0], triangles.size());

	CPhysFile *physFile = new CPhysFile();
	CPhysConvexCompoundShape *compShape = new CPhysConvexCompoundShape();
	if(params->GetPhysCompoundShape())
	{
		for(size_t i = 0; i < boneList.size(); i++) //iterate over all bones in the boneList
		{
			const string &boneName = boneList[i].getName();
			CPhysConvexHull *hull = new CPhysConvexHull();
			for(size_t k = 0; k < parentBones.size(); k++) //iterate over all vertexes parent bone ids
			{
				//only assign vertexes to the current hull that have the id we search for
				if( parentBones[k] == boneList[i].getId() ) 
				{
					Vector3f vert(triangles[k/3].vertex3[k%3].pos3);
					if( !hull->containsVertex(vert, 0.01f) )
						hull->AddVertex(vert);
				}
			}

			//only add valid hulls
			if(hull->GetSize() > 3)
				compShape->AddShape(hull, params->GetPhysCompoundShapeMaterial(boneName));
			else
				delete hull;
		}
	}
	else
	{
		CPhysConvexHull *hull = new CPhysConvexHull();
		for(const rpm_triangle_t &tri : triangles)
			for(int i = 0; i < 3; i++)
				hull->AddVertex(Vector3f(tri.vertex3[i].pos3));
		compShape->AddShape(hull, params->GetPhysDefaultMaterial());
	}

	compShape->CalculateAABB();

	//only add valid compound shape
	if(compShape->GetSize() > 0)
		physFile->AddShape(compShape);
	else
		delete compShape;

	physFile->SetCenterOfMass(params->GetPhysCenterOfMass());
	physFile->SetMass(params->GetPhysMass());

	cout << "Writing physics file: \"" << params->GetPhysOutputFile() << "\"" << endl;
	error_e err = physFile->Serialize(params->GetPhysOutputFile().c_str());
	if(err != ERR_NONE)
		wcout << "Error processing physics: " << ToString(err) << endl;
	delete physFile;
	return err;
}

#define ARGLOC_INPUT_FILE	1
#define ARGLOC_OUTPUT_FILE	2
#define ARGLOC_TEX_PREFIX	3
#define ARGLOC_SCALE		4
#define ARGLOC_ROTATION		5


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	//Disable buffering
	setvbuf( stdout, NULL, _IONBF, 0 );
	setvbuf( stderr, NULL, _IONBF, 0 );
	
	bool bScriptMode = false;
	CRPMParams *params = NULL;

	wcout << L"-----------------------------------------------------\n";
	wcout << L"Valve SMD to RedPuma model (.rpm) converter\n";
	wcout << L"Copyright (c) 2012 Marius \"RedPuma\" Gräfe\n";
	wcout << L"Build: " << __DATE__ << ", " << __TIME__ << "\n";
	wcout << "------------------------------------------------------\n\n";

	StartTimer();

	if( argc != ARGLOC_INPUT_FILE+1 && argc != ARGLOC_OUTPUT_FILE+1 && argc != ARGLOC_TEX_PREFIX+1 && argc != ARGLOC_SCALE+1 && argc != ARGLOC_ROTATION+3 )
	{
		wcout<<	"\nUsage: studiorpm <input file (smd)> <output file (rpm)> [<texture location prefix>] [scale-factor] [<rot-x> <rot-y> <rot-z>]\n"
			    "     OR: studiorpm <input script file>" << endl;
		QUIT;
	}
	
	bScriptMode = argc == ARGLOC_INPUT_FILE+1;

	if( bScriptMode )
	{
		string script_filename;
		w2a(argv[ARGLOC_INPUT_FILE], script_filename); //TODO: unicode paths
		CRPMScriptParser *parser = new CRPMScriptParser();
		params = parser;
		error_e err = parser->ParseFile(script_filename.c_str());
		if( err != ERR_NONE )
		{
			cout << "Error parsing script file: " << ToString(err);
			QUIT;
		}
	}
	else
	{
		switch(argc)
		{
		case ARGLOC_OUTPUT_FILE+1:
			params = new CRPMParamInput(argv[ARGLOC_INPUT_FILE], argv[ARGLOC_OUTPUT_FILE]);
			break;
		case ARGLOC_TEX_PREFIX+1:
			params = new CRPMParamInput(argv[ARGLOC_INPUT_FILE], argv[ARGLOC_OUTPUT_FILE], argv[ARGLOC_TEX_PREFIX]);
			break;
		case ARGLOC_SCALE+1:
			params = new CRPMParamInput(argv[ARGLOC_INPUT_FILE], argv[ARGLOC_OUTPUT_FILE], argv[ARGLOC_TEX_PREFIX], argv[ARGLOC_SCALE]);
			break;
		case ARGLOC_ROTATION+3:
			params = new CRPMParamInput(argv[ARGLOC_INPUT_FILE], argv[ARGLOC_OUTPUT_FILE], argv[ARGLOC_TEX_PREFIX], argv[ARGLOC_SCALE],
				wstring(argv[ARGLOC_ROTATION])+argv[ARGLOC_ROTATION+1]+argv[ARGLOC_ROTATION+2]);
			break;
		default: throw std::exception("Y U NO WRITE CORRECT CODE?");
		}
	}

	//Open input file
	wcout<<"Opening file \"" << params->GetModelInputFile().c_str() << "\"\n";
	std::unique_ptr<CInputFile> inputFile(createFileObject(params->GetModelInputFile()));
	if(inputFile == NULL)
		QUIT;

	wcout << "Parsing file\n";
	error_e e;
	if( (e = inputFile->parse(params->GetModelInputFile())) != ERR_NONE )
	{
		wcout << "Error: " << ToString(e) << "\n";
		QUIT;
	}

	wcout<<"Parsed " << inputFile->triangles().size() << " triangles with textures: ";
	for( size_t i = 0; i < inputFile->textures().size(); i++ )
		wcout << "\"" << inputFile->textures()[i].c_str() << "\" ";

	//create rpmfile
	rpm_file_t rpmfile;
	assembleTriangles(inputFile->triangles(), rpmfile);
	assembleTextureNames(inputFile->textures(), rpmfile, params);

	//Scale Model
	wcout<<"Scaling model to scale " << params->GetModelScale() << "\n";
	ScaleRPM( rpmfile, params->GetModelScale());

	//Calculate vertex-normals
	if(params->RecalcNormals())
	{
		wcout<<"Re-calculating Vertex-Normals\n";
		for( UINT i = 0; i < rpmfile.header.num_of_triangles * 3; i++ )
			ComputeVertexNormal( &rpmfile, i );
	}

	wcout<<"Rotating by standard rotation (-90 deg. around X)\n";
	//this one is the standard rotation
	RotateVertsAroundXAxis( -90, 
		(rpm_triangle_t*)rpmfile.findLump(LUMP_TRIANGLES)->getData(),
		rpmfile.header.num_of_triangles );

	//this one is the user-specified rotation on top of the standard one.
	if( params->GetModelRotate() != vec3_null )
	{
		wcout << "Rotating model by user defined rotation\n";
		RotateVertsByAngles( params->GetModelRotate(),
			(rpm_triangle_t*)rpmfile.findLump(LUMP_TRIANGLES)->getData(),
			rpmfile.header.num_of_triangles );
	}

	//Calculate Bounding Box
	CalcBoundingBox(rpmfile);

	//////////////////////////
	//Save File
	//////////////////////////
	wcout<<"\nWriting File: \"" << params->GetModelOutputFileW() << "\" ... \n";
	
	ofstream os( params->GetModelOutputFileW(), ios::binary );
	if(!os.good() || rpmfile.Serialize(os) != rpm_file_t::RPM_WRITE_OK)
	{
		wcout<<"Could not write File \"" << params->GetModelOutputFileW() << "\"!\n";
		QUIT;
	}
	os.close();

	if( params->IsPhysSectionThere() )
		handlePhysics(params);

    wcout<<"Successfully saved file.\n";
	wcout<<"Whole Operation took " << StopTimer() << " seconds.\n";
	QUIT;
}





#include <vector>

//---------------------------------------------------------------
// Purpose: Experimental function
//---------------------------------------------------------------
void createPhysics( rpm_triangle_t *tris, int num_tris )
{
	std::vector<ConvexShape> shapes;
	shapes.push_back( ConvexShape(true) );
	ConvexShape &shape = shapes[0];

	for( int i = 0; i < num_tris; i++ )
	{
		//Create new shape if neccessary
		for( int k = 0; k < 3; k++ ) 
		{
			if( !shape.isPointInside(tris[i].vertex3[k].pos3, 0.001f) )
			{
				shapes.push_back(ConvexShape(true));
				shape = shapes[shapes.size()-1];
			}
		}

		CPlane plane(tris[i].vertex3[0].pos3, tris[i].norm3);
		shape.addPlane(plane);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::string &handleFilenameVariables( string &filename )
{
	std::string result;
	size_t start = 0;
	while( (start = filename.find("$(", start)) != string::npos )
	{
		size_t stop = filename.find(')', start+2);
		if(stop != string::npos)
		{
			std::string varname(filename, start+2, stop-(start+2));
			char buf[512] = {0};
			DWORD ret = GetEnvironmentVariableA(varname.c_str(), buf, ARRAY_LENGTH(buf));
			if(!ret)
			{
				wcout << "WARNING: Could not resolve environment variable \"" << varname.c_str() << "\".\n";
				buf[0] = 0;
			}
			filename.replace(start, stop-start+1, buf);
			start = 0;
		}
		else
		{
			wcout << "WARNING: Possible syntax error in filename.\n";
			start += 2;
		}
	}

	return filename;
}

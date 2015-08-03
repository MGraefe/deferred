// renderer/COceanRenderer.cpp
// renderer/COceanRenderer.cpp
// renderer/COceanRenderer.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "glheaders.h"
#include "COceanRenderer.h"
#include "IOceanSolver.h"
#include "COceanFFTSolverCPU.h"
#include <util/CConVar.h>
#include <util/timer.h>
#include "CViewFrustum.h"
#include "CCamera.h"
#include "CShaders.h"

CConVar cv_texsize(		"render.water.texsize",			"512");
CConVar cv_gridlength(	"render.water.gridlength",		"2000.0");
CConVar cv_winddir_x(	"render.water.winddir.x",		"1.0");
CConVar cv_winddir_y(	"render.water.winddir.y",		"1.0");
CConVar cv_windspeed(	"render.water.windspeed",		"80.0");
CConVar cv_waveHeight(	"render.water.waveheight",		"100.0");
CConVar cv_waveSpeed(	"render.water.wavespeed",		"3.0");
CConVar cv_meshSize(	"render.water.mesh.vertcount",	"128");
CConVar cv_renderSize(	"render.water.mesh.rendersize", "16384.0");
CConVar cv_quadtreeMinSize("render.water.mesh.minsize", "512.0");
CConVar cv_texscale(	"render.water.texscale",		"512.0");
CConVar cv_dirDepend(	"render.water.dirdepend",		"0.07");

class COceanQuadtreeNode
{
public:
	COceanQuadtreeNode(const Vector2f &min, const Vector2f &max) :
		m_min(min),
		m_max(max)
	{
		
	}
	
	void SplitCube()
	{
		Vector2f cen = GetCenter();
		m_children.push_back(COceanQuadtreeNode(m_min, cen));
		m_children.push_back(COceanQuadtreeNode(Vector2f(cen.x, m_min.y), Vector2f(m_max.x, cen.y)));
		m_children.push_back(COceanQuadtreeNode(Vector2f(m_min.x, cen.y), Vector2f(cen.x, m_max.y)));
		m_children.push_back(COceanQuadtreeNode(cen, m_max));
	}

	Vector2f GetCenter()
	{
		return (m_min + m_max) * 0.5f;
	}

	float GetSize()
	{
		return m_max.x - m_min.x;
	}

	void BuildRec(float minSize, const Vector2f &sceneCenter)
	{
		m_children.clear();
		if(GetSize() > minSize - 0.1f)
		{
			float maxDist = (float)(3.0/4.0 * M_SQRT2 * GetSize());
			float myDist = (GetCenter()-sceneCenter).length();
			if(myDist < maxDist + 0.1f)
			{
				SplitCube();
				for(COceanQuadtreeNode &child : m_children)
					child.BuildRec(minSize, sceneCenter);
			}
		}
	}

	//move the whole quad-tree in steps of minSize
	void Center(const Vector2f &center, float minSize, float maxSize)
	{
		Vector2f c;
		c.x = RoundToNearestMultiple(center.x, minSize);
		c.y = RoundToNearestMultiple(center.y, minSize);
		if( (GetCenter()-c).length() > 0.1f ) //discard fp errors
		{
			float msh = maxSize * 0.5f;
			m_min = c - Vector2f(msh, msh);
			m_max = c + Vector2f(msh, msh);
			RecalcChildPositions();
		}
	}

	void RenderMeshes(const CViewFrustum *frustum, COceanRenderer *oceanRenderer, float miny, float maxy)
	{
		if(frustum->AxisAlignedCubeInFrustum(Vector3f(m_min.x, miny, m_min.y), Vector3f(m_max.x, maxy, m_max.y)))
		{
			if(m_children.empty())
				oceanRenderer->RenderMesh(GetCenter(), GetSize()*0.5f);
			else
				for(COceanQuadtreeNode &child : m_children)
					child.RenderMeshes(frustum, oceanRenderer, miny, maxy);
		}
	}

private:
	void RecalcChildPositions()
	{
		if(!m_children.empty())
		{
			Vector2f cen = GetCenter();
			m_children[0].m_min = m_min;
			m_children[0].m_max = cen;
			m_children[0].RecalcChildPositions();

			m_children[1].m_min = Vector2f(cen.x, m_min.y);
			m_children[1].m_max = Vector2f(m_max.x, cen.y);
			m_children[1].RecalcChildPositions();

			m_children[2].m_min = Vector2f(m_min.x, cen.y);
			m_children[2].m_max = Vector2f(cen.x, m_max.y);
			m_children[2].RecalcChildPositions();

			m_children[3].m_min = cen;
			m_children[3].m_max = m_max;
			m_children[3].RecalcChildPositions();
		}
	}

private:
	Vector2f m_min;
	Vector2f m_max;
	std::vector<COceanQuadtreeNode> m_children;
};



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
COceanRenderer::COceanRenderer()
{
	m_texture = 0;
	m_meshVertBuffer = 0;
	m_meshIndexBuffer = 0;
	m_meshSize = 0;
	m_meshIndexCount = 0;
	m_waterHeight = 0.0f;
	m_quadtreeParent = NULL;
	m_renderSize = 1024.0f;
	m_quadtreeMinSize = 32.0f;
	m_shader = NULL;
	m_solver = new COceanFFTSolverCPU();
	m_pixelBuffers[0] = m_pixelBuffers[1] = 0;
	m_pixelBufferIndex = 0;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
COceanRenderer::~COceanRenderer()
{
	if(m_texture != 0)
		glDeleteTextures(1, &m_texture);
	if(m_solver)
		delete m_solver;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanRenderer::InitConvars()
{
	m_N = cv_texsize.GetInt();
	m_L = cv_gridlength.GetFloat();
	m_meshSize = cv_meshSize.GetInt();
	m_quadtreeMinSize = cv_quadtreeMinSize.GetFloat();
	m_renderSize = cv_renderSize.GetFloat();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanRenderer::Init()
{
	InitConvars();

	if(m_texture != 0)
		glDeleteTextures(1, &m_texture);
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_N, m_N, 0, GL_RED, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	std::vector<float> zeroBuffer(GetDataSize()/sizeof(float), 0.0f);
	for(int i = 0; i < 2; i++)
	{
		if(m_pixelBuffers[i] != 0)
			glDeleteBuffers(1, &m_pixelBuffers[i]);
		glGenBuffers(1, &m_pixelBuffers[i]);
		if(m_pixelBuffers[i] != 0)
		{
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pixelBuffers[i]);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, GetDataSize(), &zeroBuffer[0], GL_STREAM_DRAW);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}
		else
			error("COceanRenderer: Error creating pbo buffers");
	}
	zeroBuffer.clear();
	zeroBuffer.shrink_to_fit();

	Vector2f windDir(cv_winddir_x.GetFloat(), cv_winddir_y.GetFloat());
	m_solver->Init(m_N, m_L, windDir.GetNormalized(), cv_windspeed.GetFloat(), cv_waveHeight.GetFloat(), cv_dirDepend.GetFloat());
	m_solver->Execute((float)g_RenderInterf->GetTimer()->GetTime());

	InitMesh();

	if(m_quadtreeParent)
		delete m_quadtreeParent;
	m_quadtreeParent = new COceanQuadtreeNode(Vector2f(-m_renderSize, -m_renderSize), Vector2f(m_renderSize, m_renderSize));
	m_quadtreeParent->BuildRec(m_quadtreeMinSize, vec2_null);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanRenderer::UploadAndBindTexture()
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
	
	if(!m_solver->IsExecuting())
	{
		//Upload pbo -> texture
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pixelBuffers[m_pixelBufferIndex]);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_N, m_N, GL_RED, GL_FLOAT, 0);

		//Bind other pbo
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pixelBuffers[1-m_pixelBufferIndex]);

		//Reset buffer data to return a pointer immediately
		glBufferData(GL_PIXEL_UNPACK_BUFFER, GetDataSize(), 0, GL_STREAM_DRAW);

		//Retrieve pointer
		float *data = (float*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		if(!data)
			error("Error mapping Ocean pbo buffer.");
		else
		{
			m_solver->SetCopyBuffer(data);
			m_solver->Execute(gpGlobals->curtime * cv_waveSpeed.GetFloat());
		}

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		m_pixelBufferIndex = 1 - m_pixelBufferIndex;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanRenderer::ReInit()
{
	Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanRenderer::RecreateBuffer( UINT &buf )
{
	if(buf != 0)
		glDeleteBuffers(1, &buf);
	glGenBuffers(1, &buf);
	if(buf == 0)
		error_fatal("COceanRenderer::RecreateBuffer: creating buffer failed");
}


template<typename T>
T *GenerateTriangleStripIndices(UINT vertsX, UINT vertsY, UINT &numIds)
{
	numIds = 2 * vertsX * (vertsY - 1);
	T *ids = new T[numIds];
	int x = 0;
	int y = 0;
	int dx = 1; //direction for x
	for(int i = 0; i < (int)numIds; i++)
	{
		ids[i] = y * vertsX + x;	
		if( (i+1) % (2*vertsX) == 0 )//Reached end of row?
			dx = -dx;
		else
		{
			x = i % 2 != 0 ? x+dx : x; //inc/dec x every second vertex
			y = i % 2 == 0 ? y+1 : y-1; //flip y every vertex
		}
	}
	return ids;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanRenderer::InitMesh()
{
	//Create verts from [-1,1]x[-1,1] on the xz-plane
	int N = m_meshSize;
	Vector3f *verts = new Vector3f[N*N];
	for(int x = 0; x < N; x++)
	{
		for(int z = 0; z < N; z++)
		{
			Vector3f &out = verts[x*N+z];
			out.x = ((float)x / (float)(N-1) * 2.0f - 1.0f);
			out.y = 0.0f;
			out.z = ((float)z / (float)(N-1) * 2.0f - 1.0f);
		}
	}

	//Generate indices for triangle strip
	UINT *ids = GenerateTriangleStripIndices<UINT>(N, N, m_meshIndexCount);
	
	RecreateBuffer(m_meshVertBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_meshVertBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*N*N, verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] verts;

	RecreateBuffer(m_meshIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(UINT)*m_meshIndexCount, ids, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	delete[] ids;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanRenderer::RenderMesh( Vector2f pos, float size )
{
	m_shader->SetMeshPosScale(Vector3f(pos.x, m_waterHeight, pos.y), size);
	glDrawElements(GL_TRIANGLE_STRIP, m_meshIndexCount, GL_UNSIGNED_INT, 0);
}

CConVar cv_drawMesh("render.water.mesh.draw", "0");

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanRenderer::Render(CShaderWaterPretty *shader, const CViewFrustum *frustum, const Vector3f &center )
{
	Assert(m_meshIndexBuffer != 0);
	Assert(m_meshVertBuffer != 0);

	m_quadtreeParent->Center(Vector2f(center.x, center.z), m_quadtreeMinSize*0.25f, m_renderSize*0.5f);

	m_shader = shader;
	shader->UpdateMatrices();
	shader->SetTexFactor(1.0f / cv_texscale.GetFloat());

	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	glBindBuffer(GL_ARRAY_BUFFER, m_meshVertBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshIndexBuffer);
	GetShaderManager()->InitVertexbufferVars( VBF_POS, NULL );
	GetGLStateSaver()->Disable(GL_CULL_FACE);

	if(cv_drawMesh.GetBool())
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	m_quadtreeParent->RenderMeshes(frustum, this, m_waterHeight-20.0f, m_waterHeight+20.0f);
	
	if(cv_drawMesh.GetBool())
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GetGLStateSaver()->Enable(GL_CULL_FACE);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	rglPopClientAttrib();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanRenderer::SetWaterHeight( float height )
{
	m_waterHeight = height;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanRenderer::Pause()
{
	while(m_solver && m_solver->IsExecuting())
		CCTime::Sleep(1);
}

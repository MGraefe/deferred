// renderer/COceanFFTSolverCPU.cpp
// renderer/COceanFFTSolverCPU.cpp
// renderer/COceanFFTSolverCPU.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "COceanFFTSolverCPU.h"
#include <fftw3.h>
#include <util/timer.h>
#include <iostream>
#include <fstream>
#include <util/CConVar.h>

bool g_dumpHeightfield = false;
DECLARE_CONSOLE_COMMAND(dump_heightfield, "render.water.dumpht")
{
	g_dumpHeightfield = true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool SaveBitmapP2( const char *filename, BYTE * bmpBytes, size_t width, size_t height )
{
#ifdef _WINDOWS
	Assert( (width & (width-1)) == 0 ); //Assert power of two
	Assert( (height & (height-1)) == 0 ); //Assert power of two

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;
	memset( &bmfh, 0, sizeof(bmfh) );
	memset( &info, 0, sizeof(info) );
	bmfh.bfType = 0x4d42; //0x4d42 = 'BM' little endian!
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width*height*3;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;
	info.biBitCount = 24;
	info.biCompression = BI_RGB;
	info.biXPelsPerMeter = 0x0ec4;
	info.biYPelsPerMeter = 0x0ec4;

	std::ofstream os(filename, std::ios::binary);
	if(!os.is_open())
		return false;
	os.write((char*)&bmfh, sizeof(bmfh));
	os.write((char*)&info, sizeof(info));
	os.write((char*)bmpBytes, width*height*3);
	return os.good();
#else
	return false;
#endif
}




//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
COceanFFTSolverCPUThread::COceanFFTSolverCPUThread(COceanFFTSolverCPU *solver) :
	CThread("oceansolver")
{
	m_solver = solver;
	m_executing = false;
	m_quit = false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPUThread::ThreadRun( void )
{
	while(!m_quit)
	{
		while(!m_executing && !m_quit)
			CCTime::Sleep(1);
		if(!m_quit)
		{
			m_solver->ExecuteThread(m_t);
			m_executing = false;
		}
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPUThread::SetTime( float t )
{
	m_t = t;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPUThread::Quit()
{
	m_quit = true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool COceanFFTSolverCPUThread::IsExecuting()
{
	return m_executing;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPUThread::StartWorker()
{
	m_executing = true;
}





//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
COceanFFTSolverCPU::COceanFFTSolverCPU()
{
	m_plan = NULL;
	m_h0 = NULL;
	m_hf = NULL;
	m_ht = NULL;
	m_hfr = NULL;
	m_copyBuffer = NULL;
	m_workerThread = new COceanFFTSolverCPUThread(this);
	m_timePassedMs = 1.0;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
COceanFFTSolverCPU::~COceanFFTSolverCPU()
{
	m_workerThread->Quit();
	while(m_workerThread->IsRunning())
		CCTime::Sleep(1);
	if(m_plan)
		fftwf_destroy_plan(m_plan);
	delete m_workerThread;
}


#define RECREATE_ARRAY(val, type) if(val) free(val); val = (type*)malloc(m_N*m_N*sizeof(type)); Assert(IsAligned(val, 16))
//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPU::Init( int N, float L, Vector2f windDir, float windSpeed, float waveHeight, float dirDepend )
{
	while(m_workerThread->IsExecuting())
		CCTime::Sleep(1);
	m_L = L;
	m_N = N;
	m_windDir = windDir;
	m_windSpeed = windSpeed;
	m_waveHeight = waveHeight;
	m_dirDepend = dirDepend;
	m_copyBuffer = NULL;
	RECREATE_ARRAY(m_h0, ComplexNf);
	RECREATE_ARRAY(m_ht, ComplexNf);
	RECREATE_ARRAY(m_hf, ComplexNf);
	RECREATE_ARRAY(m_hfr, float);
	if(m_plan)
		fftwf_destroy_plan(m_plan);
	m_plan = fftwf_plan_dft_2d(N, N, (fftwf_complex*)m_ht, (fftwf_complex*)m_hf, FFTW_BACKWARD, FFTW_ESTIMATE);
	if(!m_workerThread->IsRunning())
		m_workerThread->Start(); //This just creates the worker, does not start the calculation
	generateH0();
}

float g_oceanFFTSolverTimeMs = 11.0f;

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPU::ExecuteThread(float t)
{
	/*double startTime = g_RenderInterf->GetTimer()->GetTime();*/
	generateHt(t);
	double startTime = g_RenderInterf->GetTimer()->GetTime();
	fftwf_execute(m_plan);
	
	float scale = 1.0f / float(m_N*m_N);
#ifndef OCEAN_SSE
	for(int i = 0; i < m_N*m_N; i++)
		m_hfr[i] = m_hf[i].r * scale;
#else
	m128 s = _mm_set_ps1(scale);
	for(int i = 0; i < m_N*m_N; i += 4)
	{
		m128 h = _mm_set_ps(m_hf[i+3].r, m_hf[i+2].r, m_hf[i+1].r, m_hf[i].r);
		_mm_store_ps(&m_hfr[i], _mm_mul_ps(h, s));
	}
#endif

	if(m_copyBuffer)
		memcpy(m_copyBuffer, m_hfr, m_N*m_N*sizeof(float));

	double endTime = g_RenderInterf->GetTimer()->GetTime();
	m_timePassedMs = m_timePassedMs * 0.99f + (float)((endTime - startTime) * 1000.0) * 0.01f;
	g_oceanFFTSolverTimeMs = m_timePassedMs;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPU::Execute(float t)
{
	m_workerThread->SetTime(t);
	m_workerThread->StartWorker();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float COceanFFTSolverCPU::omega(float K)
{
	return sqrt(K*float(9.81));
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPU::h_tilda(int x, int y, float K, float t, ComplexNf &out)
{
	float omt = omega(K) * t;
	float sinOmt = sinf(omt);
	float cosOmt = cosf(omt);

	ComplexNf h0pk = getH0(x, y);
	//ComplexNf h0nk = getH0(clamp(m_N - x, 0, m_N-1), clamp(m_N - y, 0, m_N-1));
	ComplexNf h0nk = getH0(m_N - x - 1, m_N - y - 1);
	ComplexNf v1 = h0pk * ComplexNf(cosOmt, sinOmt);
	ComplexNf v2 = h0nk.GetConj() * ComplexNf(cosOmt, -sinOmt);
	out = v1 + v2;
}


#ifdef OCEAN_SSE
#define USE_SSELIB_SINCOS
//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPU::h_tildaSSE(float ft)
{
	m128 g = _mm_set_ps1(9.81f);
	m128 t = _mm_set_ps1(ft);
	SSE_ALIGN16_BEG float fvr[4] SSE_ALIGN16_END;
	SSE_ALIGN16_BEG float fvi[4] SSE_ALIGN16_END;
#ifndef USE_SSELIB_SINCOS
	SSE_ALIGN16_BEG float fomt[4] SSE_ALIGN16_END;
	SSE_ALIGN16_BEG float fsinOmt[4] SSE_ALIGN16_END;
	SSE_ALIGN16_BEG float fcosOmt[4] SSE_ALIGN16_END;
#endif

	for(int y = 0; y < m_N; y++)
	{
		for(int x = 0; x < m_N; x += 4)
		{
			if(x == m_N/2 && y == m_N/2)
			{
				m_ht[y*m_N+x].r = m_ht[y*m_N+x].i = 0.0f;
				continue;
			}

			m128 kx = getKx(x);
			m128 ky = getKy(y);
			m128 kl = _mm_sqrt_ps( _mm_add_ps( _mm_mul_ps(kx,kx), _mm_mul_ps(ky,ky) ) );
			m128 omt = _mm_mul_ps(t, _mm_sqrt_ps(_mm_mul_ps(kl, g)));
#ifndef USE_SSELIB_SINCOS
			_mm_store_ps(fomt, omt);
			fsinOmt[0] = sin(fomt[0]); fcosOmt[0] = cos(fomt[0]);
			fsinOmt[1] = sin(fomt[1]); fcosOmt[1] = cos(fomt[1]);
			fsinOmt[2] = sin(fomt[2]); fcosOmt[2] = cos(fomt[2]);
			fsinOmt[3] = sin(fomt[3]); fcosOmt[3] = cos(fomt[3]);
			m128 sinOmt = _mm_load_ps(fsinOmt);
			m128 cosOmt = _mm_load_ps(fcosOmt);
#else
			m128 sinOmt, cosOmt;
			sse_trig::sincos_ps(omt, &sinOmt, &cosOmt);
#endif

			int ip = y * m_N + x;
			m128 h0pkr = _mm_set_ps(m_h0[ip].r, m_h0[ip+1].r, m_h0[ip+2].r, m_h0[ip+3].r);
			m128 h0pki = _mm_set_ps(m_h0[ip].i, m_h0[ip+1].i, m_h0[ip+2].i, m_h0[ip+3].i);
			m128 v1r = _mm_sub_ps(_mm_mul_ps(h0pkr, cosOmt), _mm_mul_ps(h0pki, sinOmt));
			m128 v1i = _mm_add_ps(_mm_mul_ps(h0pkr, sinOmt), _mm_mul_ps(h0pki, cosOmt));

			int in = (m_N - y - 1) * m_N + (m_N - x - 1);
			m128 h0nkr = _mm_set_ps(m_h0[in].r, m_h0[in-1].r, m_h0[in-2].r, m_h0[in-3].r);
			m128 h0nki = _mm_set_ps(m_h0[in].i, m_h0[in-1].i, m_h0[in-2].i, m_h0[in-3].i);
			m128 v2r = _mm_sub_ps(_mm_mul_ps(h0nkr, cosOmt), _mm_mul_ps(h0nki, sinOmt));
			m128 v2i = _mm_add_ps(_mm_mul_ps(h0nkr, sinOmt), _mm_mul_ps(h0nki, cosOmt));

			_mm_store_ps(fvr, _mm_add_ps(v1r, v2r));
			_mm_store_ps(fvi, _mm_sub_ps(v1i, v2i));

			//shift the image by it's half
			int sx = (x + m_N/2) % m_N;
			int sy = (y + m_N/2) % m_N;
			int s = sy * m_N + sx;
			m_ht[s].r = fvr[3]; m_ht[s+1].r = fvr[2]; m_ht[s+2].r = fvr[1]; m_ht[s+3].r = fvr[0];
			m_ht[s].i = fvi[3]; m_ht[s+1].i = fvi[2]; m_ht[s+2].i = fvi[1]; m_ht[s+3].i = fvi[0];
		}
	}
}
#endif


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPU::generateHt(float t)
{
#ifdef OCEAN_SSE
	h_tildaSSE(t);
#else
	for(int j = 0; j < m_N; j++)
	{
		for(int i = 0; i < m_N; i++)
		{
			int sx = (i + m_N/2) % m_N;
			int sy = (j + m_N/2) % m_N;
			int s = sy * m_N + sx;
			ComplexNf &out = m_ht[s];
			if(i != m_N/2 || j != m_N/2)
			{
				Vector2f k = getK(i, j);
				float K = k.length();
				h_tilda(i, j, K, t, out);
			}
			else
			{
				out.r = out.i = 0.0;
			}
		}
	}
#endif

	if(g_dumpHeightfield)
	{
		g_dumpHeightfield = false;
		SafeHeightFieldToBitmap("heightdump.bmp");
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector2f COceanFFTSolverCPU::getK(int x, int y)
{
	int mnh = m_N/2;
	float m = float(x - mnh);
	float n = float(y - mnh);
	float f = 2.0f * PI / m_L;
	return Vector2f(m * f, n * f);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
#ifdef OCEAN_SSE
m128 COceanFFTSolverCPU::getKx( int x )
{
	SSE_ALIGN16_BEG float fx[4] SSE_ALIGN16_END = {(float)(x), (float)(x+1), (float)(x+2), (float)(x+3)};
	float fmnh = (float)(m_N/2);
	float f2pidL = 2.0f * PI / m_L;
	m128 m = _mm_sub_ps(_mm_load_ps(fx), _mm_set_ps1(fmnh));
	return _mm_mul_ps(m, _mm_set_ps1(f2pidL));
}
#endif

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
#ifdef OCEAN_SSE
m128 COceanFFTSolverCPU::getKy( int y )
{
	float r = (float)(y - m_N/2) * 2.0f * PI / m_L;
	return _mm_set_ps1(r);
}
#endif


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPU::generateH0()
{
	for(int j = 0; j < m_N; j++)
	{
		for(int i = 0; i < m_N; i++)
		{
			ComplexNf &out = getH0(i, j);
			Vector2f k = getK(i, j);
			h_zero(k, out);
		}
	}

	//SaveH0ToBitmap("h0.bmp");
}


float phillips(Vector2f Kn, float kSq, Vector2f W, float v, float a, float dir_depend)
{
	// largest possible wave from constant wind of velocity v
	float l = v * v / 9.81f;

	float kCos = Kn.Dot(W);
	float ph = a * exp(-1.0f / (l*l*kSq)) / (kSq*kSq) * (kCos*kCos);

	// filter out waves moving opposite to wind
	if (kCos < 0.0f)
		ph *= dir_depend;

	// damp out waves with very small length w << l
	float w = l / 1000.0f;
	ph *= expf(-kSq * w * w);

	return ph;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPU::h_zero( Vector2f K, ComplexNf &out )
{
	float phSqrt = 0.0f;
	if( K.x != 0.0f || K.y != 0.0f )
	{
		float kSq = K.Dot(K);
		Vector2f Kn = K.GetNormalized();
		phSqrt = sqrt(phillips(Kn, kSq, m_windDir, m_windSpeed, m_waveHeight, m_dirDepend));
	}
	float a = (1.0f / sqrt(2.0f)) * phSqrt;
	out.r = m_rnd() * a;
	out.i = m_rnd() * a;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPU::SafeHeightFieldToBitmap( const char *filename )
{
	BYTE *bmpBytes = new BYTE[m_N*m_N*3];
	for(int i = 0; i < m_N*m_N; i++)
	{
		bmpBytes[i*3+0] = bmpBytes[i*3+1] = bmpBytes[i*3+2] = 
			(BYTE)(m_hfr[i] * 255.0f + 0.5f);
	}

	SaveBitmapP2(filename, bmpBytes, m_N, m_N);
	delete[] bmpBytes;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPU::SaveHtToBitmap( const char *filename )
{
	BYTE *bmpBytes = new BYTE[m_N*m_N*3];
	for(int i = 0; i < m_N*m_N; i++)
	{
		bmpBytes[i*3+0] = 0;
		bmpBytes[i*3+1] = (BYTE)(clamp(m_ht[i].i, 0.0f, 1.0f) * 255.0f + 0.5f);
		bmpBytes[i*3+2] = (BYTE)(clamp(m_ht[i].r, 0.0f, 1.0f) * 255.0f + 0.5f);
	}

	SaveBitmapP2(filename, bmpBytes, m_N, m_N);
	delete[] bmpBytes;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPU::SaveH0ToBitmap( const char *filename )
{
	BYTE *bmpBytes = new BYTE[m_N*m_N*3];
	for(int i = 0; i < m_N*m_N; i++)
	{
		bmpBytes[i*3+0] = 0;
		bmpBytes[i*3+1] = (BYTE)(clamp(m_h0[i].i, 0.0f, 1.0f) * 255.0f + 0.5f);
		bmpBytes[i*3+2] = (BYTE)(clamp(m_h0[i].r, 0.0f, 1.0f) * 255.0f + 0.5f);
	}

	SaveBitmapP2(filename, bmpBytes, m_N, m_N);
	delete[] bmpBytes;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void COceanFFTSolverCPU::free( void *p )
{
	fftwf_free(p);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void * COceanFFTSolverCPU::malloc( size_t N )
{
	return fftwf_malloc(N);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
ComplexNf &COceanFFTSolverCPU::getH0(int x, int y)
{
	Assert(x >= 0 && x < m_N);
	Assert(y >= 0 && y < m_N);
	return m_h0[y * m_N + x];
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const float *COceanFFTSolverCPU::GetDataForUpload()
{
	return m_hfr;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool COceanFFTSolverCPU::IsExecuting()
{
	return m_workerThread->IsExecuting();
}

// renderer/COceanFFTSolverCPU.h
// renderer/COceanFFTSolverCPU.h
// renderer/COceanFFTSolverCPU.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__COceanFFTSolverCPU_H__
#define deferred__renderer__COceanFFTSolverCPU_H__

#include "IOceanSolver.h"
#include <util/CThread.h>
#include "CNormalRnd.h"

#ifdef _SSE_ENABLED
#define OCEAN_SSE
#endif

extern DLL float g_oceanFFTSolverTimeMs;

class COceanFFTSolverCPU;
class COceanFFTSolverCPUThread : public CThread
{
public:
	COceanFFTSolverCPUThread(COceanFFTSolverCPU *solver);
	void StartWorker();
	bool IsExecuting();
	void Quit();
	void SetTime(float t);
	virtual void ThreadRun( void );

private:
	float m_t;
	volatile bool m_executing;
	volatile bool m_quit;
	COceanFFTSolverCPU *m_solver;
};


struct fftwf_plan_s;
class COceanFFTSolverCPU
{
	friend class COceanFFTSolverCPUThread;
private:
	COceanFFTSolverCPU(const COceanFFTSolverCPU &other) {}
	void operator=(const COceanFFTSolverCPU &other) {}

public:
	COceanFFTSolverCPU();
	~COceanFFTSolverCPU();
	void Init(int N, float L, Vector2f windDir, float windSpeed, float waveHeight, float dirDepend);
	void Execute(float t);
	const float *GetDataForUpload();
	bool IsExecuting();
	float GetTimePassedMs() const { return m_timePassedMs; }
	void SetCopyBuffer(float *buffer) { m_copyBuffer = buffer; }; //Copy heightmap output to this buffer after operation

private:
	void *malloc(size_t N);
	void free(void *p);
	void ExecuteThread(float t);
	float omega(float K);
	void h_tilda(int x, int y, float K, float t, ComplexN<float> &out);
	void generateHt(float t);
	ComplexN<float> &getH0(int x, int y);
	Vector2f getK(int x, int y);
	void generateH0();
	void h_zero(Vector2f K, ComplexNf &out);
	void SaveH0ToBitmap(const char *filename);
	void SaveHtToBitmap(const char *filename);
	void SafeHeightFieldToBitmap(const char *filename);

#ifdef OCEAN_SSE
	void h_tildaSSE(float t);
	inline m128 getKx(int x);
	inline m128 getKy(int y);
#endif

private:
	fftwf_plan_s *m_plan;
	int m_N;
	float m_L;
	ComplexNf *m_h0; //initial spectrum
	ComplexNf *m_ht; //spectrum at time t
	ComplexNf *m_hf; //complex height-map at time t
	float *m_hfr;    //real-only height-map at time t
	COceanFFTSolverCPUThread *m_workerThread;
	Vector2f m_windDir;
	float m_windSpeed;
	float m_waveHeight;
	CNormalRnd<float> m_rnd;
	float m_timePassedMs;
	float m_dirDepend;
	float *m_copyBuffer;
};


#endif // deferred__renderer__COceanFFTSolverCPU_H__

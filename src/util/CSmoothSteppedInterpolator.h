// util/CSmoothSteppedInterpolator.h
// util/CSmoothSteppedInterpolator.h
// util/CSmoothSteppedInterpolator.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__CSmoothSteppedInterpolator_H__
#define deferred__util__CSmoothSteppedInterpolator_H__

#include <util/CSteppedInterpolator.h>

template<typename DataT, typename FactorT = float>
class CSmoothSteppedInterpolator
{
public:
	CSmoothSteppedInterpolator(float timeStep, int maxSteps, FactorT alpha, DataT initial = DataT()) :
		m_timeStep(timeStep),
		m_lastTime(0.0f),
		m_lastVal(initial),
		m_interp(timeStep, maxSteps, alpha, initial)
	{

	}

	void SetNew(const DataT &value)
	{
		m_interp.SetNew(value);
	}

	DataT Update(float time)
	{
		float timePassed = time - m_interp.GetTimeStamp();
		float progress = clamp(timePassed / m_timeStep, 0.0f, 1.0f);
		DataT currentVal = m_lastVal * (1.0f-progress) + m_interp.GetCurrent() * progress;
		m_lastVal = m_interp.Update(time);
		return currentVal;
	}

	void SetAlpha(FactorT alpha)
	{
		m_interp.SetAlpha(alpha);
	}

private:
	float m_timeStep;
	float m_lastTime;
	DataT m_lastVal;
	CSteppedInterpolator<DataT, FactorT> m_interp;
};

#endif // deferred__util__CSmoothSteppedInterpolator_H__
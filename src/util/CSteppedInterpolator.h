// util/CSteppedInterpolator.h
// util/CSteppedInterpolator.h
// util/CSteppedInterpolator.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__CSteppedInterpolator_H__
#define deferred__util__CSteppedInterpolator_H__

#include <util/CSteppedUpdater.h>

template<typename DataT, typename FactorT = float>
class CSteppedInterpolator
{
public:
	CSteppedInterpolator(float timeStep, int maxSteps, FactorT alpha, DataT initial = DataT()) :
		m_new(initial),
		m_current(initial),
		m_next(initial),
		m_last(initial),
		m_alpha(alpha),
		m_lastTime(0.0f),
		m_lastSteps(1),
		m_updater(timeStep, maxSteps)
	{

	}

	void SetNew(const DataT &value) 
	{
		m_new = value;
	}

	DataT GetCurrent() const
	{
		return m_current;
	}

	DataT Update(float time)
	{
		m_updater.Update(time, *this);
		float timeStepFactor = (time - m_updater.GetLastUpdate()) / m_updater.GetTimeStep();
		m_current = interp(m_last, m_next, clamp(timeStepFactor, 0.0f, 1.0f));		
		return m_current;
	}

	void SetAlpha(FactorT alpha)
	{
		m_alpha = alpha;
	}

	void operator()(float time)
	{
		m_last = m_next;
		m_next = interp(m_next, m_new);
	}

	float GetTimeStamp() const
	{
		return m_updater.GetLastUpdate();
	}


private:
	DataT interp(const DataT &oldVal, const DataT &newVal) const
	{
		return interp(oldVal, newVal, m_alpha);
	}

	static DataT interp(const DataT &oldVal, const DataT &newVal, FactorT alpha)
	{
		return oldVal * (FactorT(1) - alpha) + newVal * alpha; 
	}

	DataT m_new;
	DataT m_current;
	DataT m_next;
	DataT m_last;
	FactorT m_alpha;
	float m_lastTime;
	int m_lastSteps;
	CSteppedUpdater<float> m_updater;
};

#endif // deferred__util__CSteppedInterpolator_H__
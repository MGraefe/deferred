// util/CSteppedUpdater.h
// util/CSteppedUpdater.h
// util/CSteppedUpdater.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__CSteppedUpdater_H__
#define deferred__util__CSteppedUpdater_H__

template<typename TimeT = float>
class CSteppedUpdater
{
public:
	CSteppedUpdater(TimeT timeStep, int maxSteps) :
		m_timeStep(timeStep),
		m_lastUpdate(),
		m_maxSteps(maxSteps),
		m_initialized(false)
	{

	}

	//TODO: could use variadic templates to relay arguments, do that 
	//when they are supported by the compiler
	//Returns number of steps that have been done
	template<class FuncT>
	int Update(TimeT time, FuncT &functor)
	{
		if(!m_initialized)
		{
			m_initialized = true;
			m_lastUpdate = time;
			functor(time);
			return 1;
		}
		else
		{
			int steps = 0;
			while(steps < m_maxSteps && m_lastUpdate + m_timeStep < time)
			{
				m_lastUpdate += m_timeStep;
				float lastUp = m_lastUpdate; //Prevent change in FuncT
				functor(lastUp);
				steps++;
			}
			return steps;
		}
	}

	TimeT GetTimeStep() const
	{
		return m_timeStep;
	}

	int GetMaxSteps() const
	{
		return m_maxSteps;
	}

	TimeT GetLastUpdate() const
	{
		return m_lastUpdate;
	}

private:
	TimeT m_timeStep;
	TimeT m_lastUpdate;
	const int m_maxSteps;
	bool m_initialized;
};

#endif // deferred__util__CSteppedUpdater_H__
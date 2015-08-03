// util/CSmoother.h
// util/CSmoother.h
//CSmoother.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__CSmoother_H__
#define deferred__util__CSmoother_H__

template<class TVal, class TFactor = float, class TTime = float>
class CSmoother
{
public:
	CSmoother() :
		m_smoothed(0.0f),
		m_smoothing(1.0f),
		m_lastUpdate(0.0f)
	{}

	CSmoother(const TVal &init, const TFactor &smoothing, const TTime &initTime) :
		m_smoothed(init),
		m_smoothing(smoothing),
		m_lastUpdate(initTime)
	{}

	const TVal &Update(const TTime &time, const TVal &newVal)
	{
		TTime elapsed = time - m_lastUpdate;
		m_smoothed += (newVal - m_smoothed) * (elapsed / m_smoothing);
		m_lastUpdate = time;
		return m_smoothed;
	}

	void SetSmoothing(const TFactor &smoothing)
	{
		m_smoothing = smoothing;
	}

private:
	TVal m_smoothed;
	TFactor m_smoothing;
	TTime m_lastUpdate;
};

#endif // deferred__util__CSmoother_H__
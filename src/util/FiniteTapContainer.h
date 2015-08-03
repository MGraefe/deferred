// util/FiniteTapContainer.h
// util/FiniteTapContainer.h
// util/FiniteTapContainer.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//FiniteTapContainer.h

#pragma once
#ifndef deferred__util__FiniteTapContainer_H__
#define deferred__util__FiniteTapContainer_H__


//Please note that uneven counts are faster in getMedian()
template<typename T, unsigned int _Count>
class CFiniteTapContainer
{
public:
	CFiniteTapContainer() : m_iPtr(0), m_iCount(0) { }

	inline void addTap( const T &tap ) {
		mTaps[m_iPtr] = tap;
		incrementTapCount();
	}

	inline T getAverage( void ) const {
		T sum = T(0);
		if( m_iCount < _Count )
		{
			for( unsigned int i = 0; i < m_iCount; i++ )
				sum += mTaps[i];
			return sum / T(m_iCount);
		}
		else
		{
			for( unsigned int i = 0; i < _Count; i++ )
				sum += mTaps[i];
			return sum / T(_Count);
		}
	}

	//Uneven counts are faster.
	inline T getMedian( void ) const {
		if( m_iCount == 0 )
			return T(0);
		if( _Count * sizeof(T) > 2048 ) //max. 2kb on stack.
		{
			T *taps = new T[m_iCount];
			T ret = getMedianAddr(taps);
			delete[] taps;
			return ret;
		}
		else
		{
			T taps[_Count];
			return getMedianAddr(taps);
		}
	}

	inline void clean( void ) {
		m_iCount = 0;
		m_iPtr = 0;
	}

private:
	inline T getMedianAddr( T *taps ) {
		memcpy( taps, mTaps, m_iCount*sizeof(T) );
		UINT mid = m_iCount / 2;
		if( m_iCount & 0x1 ) //uneven count of taps?
		{
			nth_element( taps, taps+mid, taps+m_iCount );
			return taps[mid];
		}
		else
		{
			sort( taps, taps+m_iCount );
			return (taps[mid-1] + taps[mid]) / T(2);
		}
	}

	inline void incrementTapCount( void ) {
		if( ++m_iPtr >= _Count ) //wrap pointer around
			m_iPtr = 0;
		if( m_iCount < _Count )
			m_iCount++;
	}

private:
	T mTaps[_Count];	//The taps
	unsigned int m_iPtr; //Wrapping pointer to unused space/next value to overwrite
	unsigned int m_iCount; //The real number of taps we maintain. Starts with 0 and reaches _Count as taps are added.
};

#endif // deferred__util__FiniteTapContainer_H__

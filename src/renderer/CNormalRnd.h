// renderer/CNormalRnd.h
// renderer/CNormalRnd.h
// renderer/CNormalRnd.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__CNormalRnd_H__
#define deferred__renderer__CNormalRnd_H__

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

template<typename T>
class CNormalRnd
{
public:
	CNormalRnd() {
		m_gen = new Generator(Engine(), Dist(0,1));
	}

	~CNormalRnd() {
		if(m_gen)
			delete m_gen;
	}

	inline T rnd() {
		return (*m_gen)();
	}

	inline T operator()(void) {
		return rnd();
	}

private:
	typedef boost::mt19937 Engine;
	typedef boost::normal_distribution<T> Dist;
	typedef boost::variate_generator<Engine, Dist> Generator;
	Generator *m_gen;
};


#endif // deferred__renderer__CNormalRnd_H__
// util/CLightProbe.h
// util/CLightProbe.h
// util/CLightProbe.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__CLightProbe_H__
#define deferred__util__CLightProbe_H__

#include "maths.h"

class CLightProbe
{
public:
	inline const Vector3f &GetPosition() const { return m_pos; }
	inline void SetPosition(const Vector3f &pos) { m_pos = pos; }

	inline float GetLight() const { return m_light; }
	inline void SetLight(float light) { m_light = light; }

private:
	Vector3f m_pos;
	float m_light;
};



#endif // deferred__util__CLightProbe_H__

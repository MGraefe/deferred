// util/AABoundingBox2D.h
// util/AABoundingBox2D.h
// util/AABoundingBox2D.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__AABoundingBox2D_H__
#define deferred__util__AABoundingBox2D_H__

template<typename T>
class AABoundBox2D_t
{
public:
	AABoundBox2D_t(Vector2f m_center, Vec);
private:
	Vector2f m_center;
	Vector2f m_size;
};

#endif // deferred__util__AABoundingBox2D_H__
// renderer/CSunFlare.h
// renderer/CSunFlare.h
// renderer/CSunFlare.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__CSunFlare_H__
#define deferred__renderer__CSunFlare_H__

#include <queue>

class CSprite;

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING

class DLL CSunFlare
{
public:
	CSunFlare();
	void Init(const char *sunTexName, float size);
	void Render(void);
	void QueryOcclusion(void);
	void ResolveOcclusion(void);
	void SetSize(float size);
	void SetTexture(const char *sunTexName);
private:
	void GenerateQuery(void);
	void RenderSprite(float size);
	std::queue<unsigned int> m_queryIds;
	float m_occlusionSize;
	float m_size;
	int m_materialId;
	float m_occlusion;
	bool m_bFirstFrame;
};

POP_PRAGMA_WARNINGS


#endif

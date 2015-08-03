// client/CGuiScoreboard.cpp
// client/CGuiScoreboard.cpp
// client/CGuiScoreboard.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CGuiScoreboard.h"
#include <renderer/CDynamicText.h>
#include "CClientLapTimeCounter.h"
#include <util/StringUtils.h>
#include "CClientInterf.h"
#include <util/CEventManager.h>
#include "CClientPlayerCar.h"

namespace gui
{

DECLARE_GUI_ELEMENT( HUD_SCOREBOARD, CGuiScoreboard );

DATADESC_GUI_BEGIN(CGuiScoreboard)
	DATA_GUI_VECTOR3(m_textPos, "text_pos");
DATADESC_GUI_END()


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CGuiScoreboard::CGuiScoreboard()
{
	m_text = NULL;
	m_isVisible = false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiScoreboard::VRenderCustom()
{
	SetDrawColor(1.0f, 1.0f, 1.0f, 1.0f);
	UpdateText();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiScoreboard::VInit()
{
	m_text = new CDynamicText();
	m_text->SetInitPos( m_textPos );
	m_text->SetFont( "FontNormalText" );
	AddChild(m_text);
	g_ClientInterf->GetClientEventMgr()->RegisterListener(ev::SCOREBOARD_SHOW, this);
	g_ClientInterf->GetClientEventMgr()->RegisterListener(ev::SCOREBOARD_HIDE, this);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiScoreboard::UpdateText()
{
	CClientLapTimeCounter *counter = g_ClientLapTimeCounter;
	if(!counter)
	{
		m_text->SetText("");
		return;
	}

	//Gather entries
	const LapTimeTableEntry *table = counter->GetTable();
	std::vector<std::pair<float, const char*>> times;
	for(CClientPlayerCar *car : CClientPlayerCar::GetPlayerCars())
	{
		float time = MAX_FLOAT;
		for(int i = 0; i < LAPTIMETABLE_NUM_ENTRIES; i++)
		{
			if(table[i].clientId == car->GetPlayerId())
			{
				time = table[i].time;
				break;
			}
		}
		times.push_back(std::make_pair(time, car->GetName()));
	}

	//Sort them by time
	std::sort(times.begin(), times.end(), 
		[](const std::pair<float, const char*> &a, const std::pair<float, const char*> &b)
	        { return a.first < b.first; });


	std::string text;

	for(size_t i = 0; i < times.size(); i++)
	{
		char line[256];
		if(times[i].first != MAX_FLOAT)
		{
			float fsecs = times[i].first;
			int mins = (int)(fsecs / 60.0f);	fsecs -= (float)(mins * 60);
			int secs = (int)fsecs;				fsecs -= (float)secs;
			int hsecs = (int)(fsecs * 100.0f + 0.5f);
			sprintf_s(line, "%s      %i:%.2i:%.2i\n",
				times[i].second, mins, secs, hsecs);
		}
		else
		{
			sprintf_s(line, "%s      -:--:--", times[i].second);
		}

		text.append(line);
	}

	m_text->SetText(text.c_str());
	counter->ResetTableChanged();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CGuiScoreboard::IsVisible()
{
	return m_isVisible;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CGuiScoreboard::HandleEvent( const IEvent *evt )
{
	if(evt->GetType() == ev::SCOREBOARD_SHOW)
	{
		m_isVisible = true;
		return true;
	}

	if(evt->GetType() == ev::SCOREBOARD_HIDE)
	{
		m_isVisible = false;
		return true;
	}

	return false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiScoreboard::UnregisterEvents( void )
{
	g_ClientInterf->GetClientEventMgr()->UnregisterListener(ev::SCOREBOARD_SHOW, this);
	g_ClientInterf->GetClientEventMgr()->UnregisterListener(ev::SCOREBOARD_HIDE, this);
}



} // namespace gui

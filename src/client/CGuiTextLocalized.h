// client/CGuiTextLocalized.h
// client/CGuiTextLocalized.h
//CGuiTextLocalized.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__CGuiTextLocalized_H__
#define deferred__client__CGuiTextLocalized_H__

#include <renderer/CDynamicText.h>
//#include "ILocaleStringReceiver.h"

namespace gui
{

class CGuiTextLocalized : public CDynamicText
{
	DECLARE_GUI_DATADESC(CDynamicText, CGuiTextLocalized);

public:
	//CGuiTextLocalized();
	//~CGuiTextLocalized();

	void SetText(const std::string &text);
	const std::string &GetTextLoc() const { return m_textLoc; }

private:
	std::string m_textLoc;
};

}

#endif // deferred__client__CGuiTextLocalized_H__
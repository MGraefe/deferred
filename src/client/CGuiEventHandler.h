// client/CGuiEventHandler.h
// client/CGuiEventHandler.h
//CGuiEventHandler.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__client__CGuiEventHandler_H__
#define deferred__client__CGuiEventHandler_H__

#include <util/IEvent.h>

namespace gui
{
class CGuiElement;

class CGuiEventHandler : public IEventListener
{
public:
	void Init();
	virtual bool HandleEvent( const IEvent *evt );
	virtual void UnregisterEvents( void );

private:
	CGuiElement *GetGuiRoot();
};

} //namespace gui

#endif // deferred__client__CGuiEventHandler_H__
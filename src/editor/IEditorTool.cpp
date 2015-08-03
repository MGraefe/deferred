// editor/IEditorTool.cpp
// editor/IEditorTool.cpp
// editor/IEditorTool.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"

#include "IEditorTool.h"
#include "editorDoc.h"

IEditorTool* IEditorTool::GetToolById( tools_e tool ) 
{ 
	const EditorToolList &tl = GetActiveDocument()->GetToolList();

	for( EditorToolList::const_iterator it = tl.begin(); it != tl.end(); it++ )
		if( (*it)->GetToolId() == tool )
			return *it;

	//When we go here something went terribly wrong
	Assert(false);
	return NULL;
}

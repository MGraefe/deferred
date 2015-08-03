// util/MessageBox.h
// util/MessageBox.h
// util/MessageBox.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__MessageBox_H__
#define deferred__util__MessageBox_H__

#ifndef _WINDOWS

#define MB_OK 		0
#define MB_YESNO	4

#define MB_ICONERROR 0
#define MB_ICONWARNING 0

#define IDOK 1
#define IDYES 6
#define IDNO 7

int MessageBoxA(void *owner = NULL, const char *text = NULL, const char *caption = NULL, int type = MB_OK);

#define MessageBox MessageBoxA

#else /* _WINDOWS */
#error DO NOT USE THIS WORKAROUND FILE IN WINDOWS
#endif

#endif /* MESSAGEBOX_H_ */

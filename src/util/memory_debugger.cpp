// util/memory_debugger.cpp
// util/memory_debugger.cpp
// util/memory_debugger.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------



#include "stdafx.h"

//_C_LIB_DECL
//	int __cdecl _callnewh(size_t size) _THROW1(_STD bad_alloc);
//_END_C_LIB_DECL
//
//void *__CRTDECL operator new(size_t size) _THROW1(_STD bad_alloc)
//{       // try to allocate size bytes
//	void *p;
//	while ((p = malloc(size)) == 0)
//		if (_callnewh(size) == 0)
//		{       // report no memory
//			_THROW_NCEE(_XSTD bad_alloc, );
//		}
//
//		return (p);
//}

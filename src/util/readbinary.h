// util/readbinary.h
// util/readbinary.h
// util/readbinary.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#define WRITE_BINARY(os, obj) (os).write((char*)&(obj), sizeof(obj))
#define READ_BINARY(is, obj) (is).read((char*)&(obj), sizeof(obj))

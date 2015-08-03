-include ../options.mk
# DO NOT EDIT ABOVE THIS POINT

# add additional compiler flags via += NOT =
CXXFLAGS += -I../ -D_RENDERER -I/usr/include/freetype2

# directory where .o and .d files are stored (ends with '/')
OBJDIR = ../../renderer/$(BUILD)/

# filename of the resulting file
OUTNAME = librenderer.so

# sources to compile
SOURCES = $(wildcard *.cpp)

# linker, $(AR) for static lib, $(LN) for dynamic lib, $(EXE) for executable
LINKER = $(LN)

#additional flags you would supply to the linker
LINKERFLAGS = -lutil -lGLEW -lfftw3f -lfreetype


# DO NOT EDIT BELOW THIS POINT
-include ../common.mk


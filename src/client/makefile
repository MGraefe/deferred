-include ../options.mk
# DO NOT EDIT ABOVE THIS POINT

# add additional compiler flags via += NOT =
CXXFLAGS += -I../ -D_CLIENT -I/usr/include/OIS

# directory where .o and .d files are stored (ends with '/')
OBJDIR = ../../client/$(BUILD)/

# filename of the resulting file
OUTNAME = libclient.so

# sources to compile
SOURCES = $(wildcard *.cpp) $(wildcard sound/*.cpp)

# linker, $(AR) for static lib, $(LN) for dynamic lib, $(EXE) for executable
LINKER = $(LN)

#additional flags you would supply to the linker
LINKERFLAGS = -lutil -lserver -lrenderer -lopenal -lOIS


# DO NOT EDIT BELOW THIS POINT
-include ../common.mk


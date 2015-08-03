-include ../options.mk
# DO NOT EDIT ABOVE THIS POINT

# add additional compiler flags via += NOT =
CXXFLAGS += -I../ -D_SERVEREXE

# directory where .o and .d files are stored (ends with '/')
OBJDIR = ../../serverexe/$(BUILD)/

# filename of the resulting file
OUTNAME = dedicated

# sources to compile
SOURCES = $(wildcard *.cpp)

# linker, $(AR) for static lib, $(LN) for dynamic lib, $(EXE) for executable
LINKER = $(EXE)

#additional flags you would supply to the linker
LINKERFLAGS = -lserver


# DO NOT EDIT BELOW THIS POINT
-include ../common.mk


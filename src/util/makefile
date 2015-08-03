-include ../options.mk
# DO NOT EDIT ABOVE THIS POINT

# add additional compiler flags via += NOT =
CXXFLAGS += -D_UTIL -I../

# directory where .o and .d files are stored (ends with '/')
OBJDIR = ../../util/$(BUILD)/

# filename of the resulting file
OUTNAME = libutil.a

# sources to compile
SOURCES = $(wildcard *.cpp)

# linker, $(AR) for static lib, $(LN) for dynamic lib, $(EXE) for executable
LINKER = $(AR)

#additional flags you would supply to the linker
LINKERFLAGS =


# DO NOT EDIT BELOW THIS POINT
-include ../common.mk


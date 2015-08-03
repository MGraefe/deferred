-include ../options.mk
# DO NOT EDIT ABOVE THIS POINT

# add additional compiler flags via += NOT =
CXXFLAGS += -I../

# directory where .o and .d files are stored (ends with '/')
OBJDIR = ../../exe/$(BUILD)/

# filename of the resulting file
OUTNAME = deferred.run

# sources to compile
SOURCES = $(wildcard *.cpp)

# linker, $(AR) for static lib, $(LN) for dynamic lib, $(EXE) for executable
LINKER = $(EXE)

#additional flags you would supply to the linker
LINKERFLAGS = -lclient


# DO NOT EDIT BELOW THIS POINT
-include ../common.mk


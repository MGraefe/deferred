-include ../options.mk
# DO NOT EDIT ABOVE THIS POINT

# add additional compiler flags via += NOT =
CXXFLAGS += -DRPAF_EXPORT

# directory where .o and .d files are stored (ends with '/')
OBJDIR = ../../filesystem/$(BUILD)/

# filename of the resulting file
OUTNAME = libfilesystem.so

# sources to compile
SOURCES = $(wildcard *.cpp)

# linker, $(AR) for static lib, $(LN) for dynamic lib, $(EXE) for executable
LINKER = $(LN)

#additional flags you would supply to the linker
LINKERFLAGS = -lboost_filesystem -lboost_regex


# DO NOT EDIT BELOW THIS POINT
-include ../common.mk


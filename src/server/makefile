-include ../options.mk
# DO NOT EDIT ABOVE THIS POINT

# add additional compiler flags via += NOT =
CXXFLAGS += -D_SERVER -I../ -I/usr/include/bullet -Wno-invalid-offsetof

# directory where .o and .d files are stored (ends with '/')
OBJDIR = ../../server/$(BUILD)/

# filename of the resulting file
OUTNAME = libserver.so

# sources to compile
SOURCES = $(wildcard *.cpp)

# linker, $(AR) for static lib, $(LN) for dynamic lib, $(EXE) for executable
LINKER = $(LN)

#additional flags you would supply to the linker
LINKERFLAGS = -lutil -lfilesystem -lBulletDynamics -lBulletCollision -lLinearMath -lpthread -lrt

# DO NOT EDIT BELOW THIS POINT
-include ../common.mk

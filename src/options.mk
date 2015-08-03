all: link

CXX = g++
CXXFLAGS = -D_LINUX -std=c++11 -fPIC -Wall -Wno-reorder -Wno-invalid-offsetof -fvisibility=hidden

ifeq ($(BUILD),debug)
CXXFLAGS += -D_DEBUG -O0 -g3
else
BUILD = release
CXXFLAGS += -O3 -march=pentium4 -mtune=core2 -DNDEBUG -D_RELEASE
endif

AR = ar
ARFLAGS = -r

LN = g++ -shared
LNFLAGS = -std=c++11 -fPIC

EXE = g++
EXEFLAGS = -std=c++11

MKDIR = mkdir -p

CP = cp
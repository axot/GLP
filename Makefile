CXX = g++
CXXFLAGS = -Wall -O3 -Wno-deprecated -fno-strict-aliasing -funroll-all-loops -fopenmp -I/usr/local/include/eigen3 -I/usr/local/include -I/usr/include/eigen3
LDFLAGS = -lglp

SRCS = GLP/Classes
BINDIR = build/Release

SOURCES	:= $(shell find $(SRCS) -name '*.cpp')
OBJECTS := $(SOURCES:.cpp=.o)

all : gspls

lib : libglp.a

gspls :	 $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) GLP/Examples/gspls.cpp
	mv gspls $(BINDIR)/

libglp.a : $(OBJECTS)
	ar r $@ $(OBJECTS)
clean :
	rm -f libglp.a
	find $(SRCS) -name '*.o' | xargs rm

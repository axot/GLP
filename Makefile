# Makefile for Linux
# for Mac OS X, use XCode instead

CPP = g++
CPPFLAGS = -Wall -O3 -Wno-deprecated -fPIC -fno-strict-aliasing -funroll-all-loops -fopenmp
CPPFLAGS += -I/usr/local/include -I/usr/include/eigen3 -I/usr/local/include/eigen3
LDFLAGS = -shared

SRCS = GLP/Classes
BINDIR = build/Release

DESTDIR = 
PREFIX = /usr/local
bindir = /bin
libdir = /lib

SOURCES	:= $(shell find $(SRCS) -name '*.cpp')
OBJECTS := $(SOURCES:.cpp=.o)

all : libglp gspls gspls_static

gspls : GLP/Examples/gspls.o
	$(CPP) $(CPPFLAGS) -o $@ $? -lglp

gspls_static : GLP/Examples/gspls.o
	$(CPP) $(CPPFLAGS) -o $@ $? -lglp -static

libglp : $(OBJECTS)
	ar r $@.a $?
	$(CPP) -o $@.so $? $(LDFLAGS)

install :
	install -m 0755 gspls $(DESTDIR)$(PREFIX)$(bindir)/gspls
	install -m 0644 libglp.so $(DESTDIR)$(PREFIX)$(libdir)/libglp.so
	install -m 0644 libglp.a $(DESTDIR)$(PREFIX)$(libdir)/libglp.a

uninstall :
	rm -f $(DESTDIR)$(PREFIX)$(bindir)/gspls
	rm -f $(DESTDIR)$(PREFIX)$(libdir)/libglp.so
	rm -f $(DESTDIR)$(PREFIX)$(libdir)/libglp.a

clean :
	rm -f *.a *.so
	rm -f gspls*
	find $(SRCS) -name '*.o' | xargs rm


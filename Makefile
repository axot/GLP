# CMake wrapper for Linux
# for Mac OS X, use XCode instead

RELEASEBUILDIR=build/GLP.build/Release
DEBUGBUILDIR=build/GLP.build/Debug

all: release

release: 
	@mkdir -p $(RELEASEBUILDIR);			\
	cd $(RELEASEBUILDIR);				\
	cmake -DCMAKE_BUILD_TYPE=Release ../../../GLP;	\
	make

debug:
	@mkdir -p $(DEBUGBUILDIR);		    	\
	cd $(DEBUGBUILDIR);				\
	cmake -DCMAKE_BUILD_TYPE=Debug ../../../GLP;	\
	make

install:
	@cd $(RELEASEBUILDIR);	\
	make install
	
clean:
	@if test -d $(RELEASEBUILDIR); then	\
		cd $(RELEASEBUILDIR);		\
		make clean;			\
	fi
	@if test -d $(DEBUGBUILDIR); then	\
	     	cd $(DEBUGBUILDIR);		\
	     	make clean;		    	\
	fi


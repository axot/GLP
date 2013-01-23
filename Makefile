# CMake wrapper 

RELEASEBUILDIR=build/GLP.build/Release
DEBUGBUILDIR=build/GLP.build/Debug

all: release

release: 
	@mkdir -p $(RELEASEBUILDIR);			\
	cd $(RELEASEBUILDIR);				\
	cmake -DCMAKE_BUILD_TYPE=Release ../../../GLP;	\
	make -j4;					\
	cd -

debug:
	@mkdir -p $(DEBUGBUILDIR);		    	\
	cd $(DEBUGBUILDIR);				\
	cmake -DCMAKE_BUILD_TYPE=Debug ../../../GLP;	\
	make -j4;					\
	cd -

install:
	@cd $(RELEASEBUILDIR);	\
	make install;		\
	cd -
	
clean:
	@if test -f $(RELEASEBUILDIR)/Makefile; then	\
		cd $(RELEASEBUILDIR);			\
		make clean;				\
		cd -;					\
	fi
	@if test -f $(DEBUGBUILDIR)/Makefile; then	\
	     	cd $(DEBUGBUILDIR);			\
	     	make clean;				\
		cd -;					\
	fi

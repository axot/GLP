# CMake wrapper 

RELEASEBUILDIR=build/GLP.build/Release
DEBUGBUILDIR=build/GLP.build/Debug

all: release

release: 
	@mkdir -p $(RELEASEBUILDIR);			\
	cd $(RELEASEBUILDIR);				\
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/Cellar/glp/1.0 ../../../GLP;	\
	make -j4;					\
	cd -

debug:
	@mkdir -p $(DEBUGBUILDIR);		    	\
	cd $(DEBUGBUILDIR);				\
	cmake -DCMAKE_BUILD_TYPE=Debug ../../../GLP;	\
	make -j4;					\
	cd -

gendoc:
	@mkdir -p build/Documentation
	@cd GLP/Doxygen;		\
	doxygen Doxyfile;		\
	cd -				\

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

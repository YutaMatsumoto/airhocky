# -----------------------------------------------------------------------------
# Run
runMain : bin/main.out
	. $(INITSCRIPT) && ./$<

# -----------------------------------------------------------------------------
# Assimp
LIBDIRASSIMP=-Lassimp/lib/
ASSIMPINCLUDEDIR_ECC=/usr/local/include/assimp
ASSIMPINCLUDE=-I$(ASSIMPINCLUDEDIR_ECC) # ecc assimp include files

# -----------------------------------------------------------------------------
# Mesh Objects
BALL=obj/ball.obj
BOARD=obj/board.obj
MeshAll=$(BALL) $(BORAD)

# -----------------------------------------------------------------------------
# Shaders
VSHADER=src/shader.vert
FSHADER=src/shader.frag
VSHADER2=src/shader2.vert
FSHADER2=src/shader2.frag

INITSCRIPT = bin/init.sh

OBJECTS = bin/lighting_technique.o bin/InitShader.o bin/technique.o bin/math_3d.o bin/texture.o  bin/InitShader.o

# ----------------------------------------------------------------------------
# Objects

bin/texture.o : src/texture.cpp
	$(CC) $(IMCFG) $(INCLUDEDIR) $(CXXFLAGS) -c $^ -o $@

bin/InitShader.o : src/InitShader.cpp
	$(CC) -c $(CXXFLAGS) $^ -o $@

# Lighting Lib

bin/technique.o : src/technique.cpp
	$(CC)	-c $(CXXFLAGS) src/technique.cpp -o $@

bin/lighting_technique.o : src/lighting_technique.cpp
	$(CC) -c $^ -o $@

bin/math_3d.o : src/math_3d.cpp
	$(CC) -c $^ -o $@ 

# -----------------------------------------------------------------------------
# ImageMagick
IMROOT=$(shell readlink -f imagemagick)
IMMAKEFILE=$(IMROOT)/Makefile
IMMAGICKPP=$(IMROOT)/Magick++
IMLPATH=$(IMMAGICKPP)/lib/
IMLIB  =$(IMMAGICKPP)/lib
IMBIN  =$(IMMAGICKPP)/bin
# ImageMagick : Object
IMBLOB=$(IMLIB)/Blob.o
IMIMAGE=$(IMLIB)/Image.o
IMOBJECTS = $(IMBLOB) $(IMIMAGE)
# ImageMagick : PKG_CONFIG_PATH
# The find command below finds all the .pc files under imagemagick
# 	find imagemagick/ -name "*.pc" 
PKG_CONFIG_PATH = $(IMROOT)/Magick++/lib
PKG_CONFIG_PATH := $(PKG_CONFIG_PATH):$(IMROOT)/magick
PKG_CONFIG_PATH := $(PKG_CONFIG_PATH):$(IMROOT)/wand
# ImageMagick : Config
DEP = $(IMOBJECTS)
IMCFGLAST=`$(IMBIN)/Magick++-config --ldflags --libs`
IMCFG=`$(IMROOT)/bin/Magick++-config --cppflags --cxxflags --cxxflags --ldflags --libs`
# ImageMagick : Include
IMINCLUDE=-I$(IMROOT)/Magick++/lib -I$(IMROOT)
# ImageMagick : Lib and LibDir
IMLIBS=-lMagick++ -lMagickCore -lMagickWand
IMLIBDIR=-L$(IMROOT)/Magick++/lib/.libs -L$(IMROOT)/magick/.libs -L$(IMROOT)/utilities/.libs -L$(IMROOT)/Magick++/lib/.libs -L$(IMROOT)/magick/.libs -L$(IMROOT)/wand/.libs

# -----------------------------------------------------------------------------
# Compiler and its Options
CC=g++
CXXFLAGS=-g -Wall -std=c++0x # -fpermissive

# -----------------------------------------------------------------------------
# Make Main
MAINDEP = src/main.cpp $(OBJECTS)
bin/main.out : src/main.cpp $(LIBSJ8) $(OBJECTS)
	@if [ ! -e $(IMMAKEFILE) ] ; then \
		make $(IMMAKEFILE) ;\
	else \
		echo -n ""; \
	fi
	$(CC) $(LIBDIR) $(INCLUDEDIR) $(CXXFLAGS) $^ $(LIBSJ8) -o $@ # $(IMCFGLAST)

# -----------------------------------------------------------------------------
# Jikken's
# $(IMCFG) 
J8DEP = src/j8.cpp $(OBJECTS) # $(BSD) $(BSC) $(BSL) $(BSS) 
bin/j8.out : src/j8.cpp $(LIBSJ8) $(OBJECTS)
	@if [ ! -e $(IMMAKEFILE) ] ; then \
		make $(IMMAKEFILE) ;\
	else \
		echo -n ""; \
	fi
	$(CC) $(LIBDIR) $(INCLUDEDIR) $(CXXFLAGS) $^ $(LIBSJ8) -o $@ # $(IMCFGLAST) 

$(IMMAKEFILE) ImageMagickLib :
	cd $(IMROOT); ./configure ; make;

# -----------------------------------------------------------------------------
# Aggregation 
# Libs
	# LPATH=-Wl,-rpath=lib # embed dynamic library (in ./lib) references into executable

LIBSJ8= -lglut -lGLEW -lGL -lGLU -lassimp $(IMLIBS)
LIBS= -lglut -lGLEW -lGL -lGLU -lassimp $(BSD) $(BSC) $(BSL) $(BSS) $(IMLIBS)
LIBDIR = $(LIBDIRASSIMP) $(IMLIBDIR)
# Include Headers
INCLUDEDIR= -I./assimp/ -Ibullet/src -I./assimp/include/assimp $(ASSIMPINCLUDE) $(IMINCLUDE) -Isrc 

# Make Debug
makedebug : 
	@echo "ImageMatickRoot=$(IMROOT)"
	@echo "PKG_CONFIG_PATH=$(PKG_CONFIG_PATH)"
	@echo "IMCFG="$(IMCFG)
	@echo "IMCFGLAST="$(IMCFGLAST)
	@echo "IMLIB="$(IMLIB)
	@echo "LD_LIBRARY_PATH="$(LD_LIBRARY_PATH)

IMMakefile = $(shell readlink -f imagemagick/Makefile)
ImageMagick : $(IMMakefile)
	cd $(IMROOT) ; ./configure && make ; cd ..

clean: 
	rm bin/*.out bin/*.o ; cd bullet ; make clean ; cd .. 

# -----------------------------------------------------------------------------
.PHONY : assimp
libassimp : assimp/Makefile
	cd assimp && make && cd ..

assimp/Makefile : assimp/CMakeLists.txt
	cd assimp && cmake -G 'Unix Makefiles' 

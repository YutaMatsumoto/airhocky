# Linux
CC=g++

# For Macs uncomment the next line and comment out the previous one
#CC=clang++
# Assuming you want to use a recent compiler

# Compiler flags
LIBS= -lglut -lGLEW -lGL -lassimp
CXXFLAGS= -g -Wall -std=c++0x

all: program

program: main.cpp
	$(CC) $(CXXFLAGS) main.cpp -o Hockey $(LIBS)

tags: main.cpp
	ctags *.h *.cpp

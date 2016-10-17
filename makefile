# GNU C/C++ compiler and linker:
CC = gcc
CXX = g++
LINK = g++
CU = nvcc

# Turn on optimization and warnings (add -g for debugging with gdb):
# CPPFLAGS = 		# preprocessor flags
CFLAGS = -O -Wall
CXXFLAGS = -O -Wall

# OpenGL/Mesa libraries for Linux:
GL_LIBS = -lglut -lGLU -lGL -lm

# OpenGL libraries for Windows (MinGW):
# GL_LIBS = -lfreeglut -lglu32 -lopengl32
# GL_LIBS = -lglut32 -lglu32 -lopengl32

# OpenGL libraries for Windows (MSVS):
# GL_LIBS = opengl32.lib glu32.lib freeglut.lib
# GL_LIBS = opengl32.lib glu32.lib glut32.lib

#-----------------------------------------------------------------------

# MAKE allows the use of "wildcards", to make writing compilation instructions
# a bit easier. GNU make uses $@ for the target and $^ for the dependencies.

all:    mandelbrot

color: color.o
	$(LINK) -o $@ $^

mandelbrot_cuda.o: mandelbrot_cuda.cu
	$(CU) -o $@ -c $<

julia_cuda.o: julia_cuda.cu
	$(CU) -o $@ -c $< 

julia: julia.o
	$(LINK) -o $@ $^ $(GL_LIBS)

mandelbrot_points: mandelbrot_points.o
	$(LINK) -o $@ $^ $(GL_LIBS) 

# remove the cu after this target to compule the cu stuff (need to 
# remove the other mandelbrot target as well). 
mandelbrotcu: julia.o mandelbrot_points.o color.o mandelbrot_cuda.o julia_cuda.o
	$(LINK) -o $@ $^ mandelbrot.cpp $(GL_LIBS) -L/usr/local/cuda/lib -lcuda -lcudart 

# to build mandelbrot while on a machine without cuda
mandelbrot: julia.o mandelbrot_points.o color.o 
	$(LINK) -o $@ $^ mandelbrot.cpp $(GL_LIBS)

# generic C and C++ targets for OpenGL programs consisting of only one file
# type "make filename" (no extension) to build
.c:
	$(CC) -o $@ $@.c $(CFLAGS) $(GL_LIBS)

.cpp:
	$(CXX) -o $@ $@.cpp $(CXXFLAGS) $(GL_LIBS)

.cu:
	$(CU) -o $@ $@.cu 


# utility targets
clean:
	rm -f *.o *~ core

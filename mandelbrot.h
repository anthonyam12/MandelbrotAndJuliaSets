//#include <iostream> 
#include <GL/freeglut.h>

#include "global.h"

using namespace std;

// Constants
const int Plus = '+';
const int Minus = '-';
const int J = 'j';
const int C = 'c';
const int R = 'r';
const int A = 'a';

// Globals
vector<ComplexPoint> MandelbrotPoints;
vector<ComplexPoint> JuliaPoints;

float MouseX = 0;
float MouseY = 0;

float ScreenWidth = 1200;
float ScreenHeight = 800;
float ViewplaneX = 1000;
float ViewplaneY = 1000;

// OpenGL Callbacks
void display( void );
void reshape( int w, int h );
void keyboard( unsigned char key, int x, int y );
void mouseclick( int button, int state, int x, int y );
void mousemove( int x, int y );

// Misc. Functions
void initOpenGL();
float mandelbrot( int x, int y );
float julia( int x, int y );

//#include <iostream> 
#include <GL/freeglut.h>
#include <math.h>

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

Julia julia;
Mandelbrot mandelbrot;
Mandelbrot_cu mandelbrotCu;

Color ColorSchemes[7];
ComplexPoint JuliaSeed;
bool IsJulia = false;

float ScreenWidth = 900;
float ScreenHeight = 900;

// OpenGL Callbacks
void display( void );
void reshape( int w, int h );
void keyboard( unsigned char key, int x, int y );
void mouseclick( int button, int state, int x, int y );
void mousemove( int x, int y );

// Misc. Functions
void initOpenGL();

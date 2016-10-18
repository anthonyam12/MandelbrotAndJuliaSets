//#include <iostream> 
#include <GL/freeglut.h>	// OpenGL utilities, window manager
#include <math.h>			// may not need - check
#include <time.h>			// random see
#include <stdlib.h>			// rand()

#include "global.h"

using namespace std;

// Structs
struct MinMax
{
	double xmin;
	double xmax;
	double ymin;
	double ymax;
};

// Constants
const int Plus = '+';
const int Minus = '-';
const int J = 'j';
const int C = 'c';
const int R = 'r';
const int A = 'a';
const int EscapeKey = 27;

// Globals
vector<ComplexPoint> MandelbrotPoints;
vector<ComplexPoint> JuliaPoints;

Julia julia;						// Class to get Julia points
Mandelbrot mandelbrot;				// Class to get Mandebrot points
//Mandelbrot_cu mandelbrotCu;			// Mandelbrot calculations on the GPU

vector< ColorScheme > ColorSchemes;	// Colors for the points
ColorScheme CurrentScheme;			// Scheme currently being used
ColorScheme BeforeAnimating;		// Used to reset after animation
ComplexPoint JuliaSeed;				// Seed for the Julia Set
bool IsJulia = false;				// Display the Julia set or Mandelbrot Set
bool Animating = false;				// Are we animating?
bool Reshape = false;				// Used in preventing reshaping

int MandelbrotStepsX = 500;
int MandelbrotStepsY = 500;
int JuliaStepsX = 500;
int JuliaStepsY = 500;

float ScreenWidth = 500;			// Window's width
float ScreenHeight = 500;			// Window's height

// OpenGL Callbacks
void display( void );
void reshape( int w, int h );
void keyboard( unsigned char key, int x, int y );
void special( int key, int x, int y );
void mouseclick( int button, int state, int x, int y );
void mousemove( int x, int y );
void update( int value );

// Misc. Functions
void initOpenGL();
void GenerateRandomColorScheme();
void CreateColorVector();
void ChangeColor();
bool EqualSchemes( ColorScheme scheme1, ColorScheme scheme2 );
MinMax GetMinMax();

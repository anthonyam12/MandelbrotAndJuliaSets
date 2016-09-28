#include <iostream> 
#include <GL/freeglut.h>

using namespace std;

// OpenGL Callbacks
void display( void );
void reshape( int w, int h );
void keyboard( unsigned char key, int x, int y );
void mouseclick( int button, int state, int x, int y );
void mousedrag( int x, int y );

// Misc. Functions
float mandelbrot( int x, int y );
float julia( int x, int y );

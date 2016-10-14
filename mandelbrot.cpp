#include "mandelbrot.h" 

int main( int argc, char* argv[] )
{
	// Initialize glut/openGL
	glutInit( &argc, argv );
	initOpenGL();

	Julia j;
	Mandelbrot m;
	ComplexPoint pt;
	vector< ComplexPoint > points;

	points = j.GetPoints(0, 0, 0, 0, 0);
	pt = points[0];
	cout << "here " << pt.y << endl;
	points = m.GetPoints(0, 0, 0, 0, 0);
	pt = points[0];
	cout << "HERERRE "<< pt.y << endl;

	// enter glut main loop
	glutMainLoop();

	/*
		"To keep the compiler from bitching."
			- Dr. John Weiss
	*/
	return 0;
}

/*******************************************************************************
 *								OpenGL Functions							   *
 ******************************************************************************/
void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT );
	
	glutSwapBuffers();
	glFlush();
}

void reshape( int w, int h )
{
	ScreenWidth = w;
	ScreenHeight = h;
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( -ViewplaneX, ViewplaneX, -ViewplaneY, ViewplaneY );
	glViewport( 0, 0, w, h );
}

void keyboard( unsigned char key, int x, int y )
{
	// +/- keys for zoom (scaling transform)
	// J - toggle between Mandelbrot and Julia Sets at Current Cursor position
	// 	   (open new window for Julia)
	// C - change color maps
	// R - generate random color map
	// A - animate by cycling between color maps
	switch ( key ) 
	{
		case Plus:
			break;
		case Minus:
			break;
		case J:
			// Calc/Open Julia set at complex point MouseX + i*MouseY
			break;
		case C:
			break;
		case R:
			break;
		case A:
			break;
	}	
}

void mouseclick( int button, int state, int x, int y )
{
	
}

void mousemove( int x, int y )
{
	// track the mouse position to open Julia Set
	// need to convert between window relative coordinates to
	// viewplane relative coordinates
	// May be useful - http://webglfactory.blogspot.com/2011/05/how-to-convert-world-to-screen.html
	MouseX = x;
	MouseY = y;
}

/*******************************************************************************
 *								Misc. Functions								   *
 ******************************************************************************/
void initOpenGL( void )
{
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
	
	// window settings 
	glutInitWindowSize( ScreenWidth, ScreenHeight );
	glutInitWindowPosition( 200, 40 );
	glutCreateWindow( "Mandelbrot and Julia Sets" );
	
	// color used to clear screen - black
	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	// callbacks
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );
	glutPassiveMotionFunc( mousemove );
}

float mandelbrot( int x, int y )
{
	return 0;
}

float julia( int x, int y )
{
	return 0;
}

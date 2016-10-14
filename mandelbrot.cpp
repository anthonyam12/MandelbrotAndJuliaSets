#include "mandelbrot.h" 

int main( int argc, char* argv[] )
{
	// Initialize glut/openGL
	glutInit( &argc, argv );
	initOpenGL();

	MandelbrotPoints = mandelbrot.GetPoints(1000, 1000, 1000);
	JuliaPoints = julia.GetPoints(1000, 1000, 1000);

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

	ComplexPoint pt;
	for( vector<ComplexPoint>::const_iterator it=MandelbrotPoints.begin(); it < MandelbrotPoints.end(); it++ )
	{
		pt = *it;
		float color[3] = { pt.color.r, pt.color.g, pt.color.b };
		glColor3fv( color );
		glBegin( GL_POINTS );
			glVertex2f( pt.x, pt.y );
		glEnd();
	}

	glFlush();
}

void reshape( int w, int h )
{
	ScreenWidth = w;
	ScreenHeight = h;

	float xmin = mandelbrot.GetComplexXMin();
	float xmax = mandelbrot.GetComplexXMax();
	float ymin = mandelbrot.GetComplexYMin();
	float ymax = mandelbrot.GetComplexYMax();

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	gluOrtho2D( xmin, xmax, ymin, ymax  );
	glViewport( 0, 0, w, h );

	glClear( GL_COLOR_BUFFER_BIT );
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
	glutInitDisplayMode( GLUT_RGB | GLUT_SINGLE );
	
	// window settings 
	glutInitWindowSize( ScreenWidth, ScreenHeight );
	glutInitWindowPosition( 200, 40 );
	glutCreateWindow( "NineteenSixtyX" );
	
	// color used to clear screen - black
	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	// callbacks
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );
	glutPassiveMotionFunc( mousemove );
}

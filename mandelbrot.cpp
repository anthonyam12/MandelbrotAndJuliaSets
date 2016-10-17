#include "mandelbrot.h" 

int manWin, julWin;

int main( int argc, char* argv[] )
{
	srand( time( NULL ) );

	CreateColorVector();
	CurrentScheme = ColorSchemes.at(0);

	// get Mandelbrot points
	MandelbrotPoints = mandelbrot.GetPoints( 1000, 1000, 1000, CurrentScheme );
	vector<ComplexPoint> test = mandelbrotCu.GetPoints(1000, 1000, 1000, NULL);

	// Initialize glut/openGL
	glutInit( &argc, argv );
	initOpenGL();

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
	//glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	float xmin = 0;
	float xmax = 0;
	float ymin = 0;
	float ymax = 0;

	if( !IsJulia )
	{
		xmin = mandelbrot.GetComplexXMin();
		xmax = mandelbrot.GetComplexXMax();
		ymin = mandelbrot.GetComplexYMin();
		ymax = mandelbrot.GetComplexYMax();
	}
	else 
	{
		xmin = julia.GetComplexXMin();
		xmax = julia.GetComplexXMax();
		ymin = julia.GetComplexYMin();
		ymax = julia.GetComplexYMax();
	}

	gluOrtho2D( xmin, xmax, ymin, ymax );
	glViewport( 0, 0, ScreenWidth, ScreenHeight );

	glClear( GL_COLOR_BUFFER_BIT );	
	glutSwapBuffers();

	vector<ComplexPoint> plotVec = IsJulia ? JuliaPoints : MandelbrotPoints;

	ComplexPoint pt;
	for( vector<ComplexPoint>::const_iterator it=plotVec.begin(); it < plotVec.end(); it++ )
	{
		pt = *it;
		float color[3] = { pt.color.r, pt.color.g, pt.color.b };
		//cout << pt.x << ", " << pt.y << endl;
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

	float xmin = 0;
	float xmax = 0;
	float ymin = 0;
	float ymax = 0;

	if( !IsJulia )
	{
		xmin = mandelbrot.GetComplexXMin();
		xmax = mandelbrot.GetComplexXMax();
		ymin = mandelbrot.GetComplexYMin();
		ymax = mandelbrot.GetComplexYMax();
	}
	else 
	{
		// set x/y min/max based on Julia values 
		xmin = julia.GetComplexXMin();
		xmax = julia.GetComplexXMax();
		ymin = julia.GetComplexYMin();
		ymax = julia.GetComplexYMax();
	}

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	gluOrtho2D( xmin, xmax, ymin, ymax  );
	//glutReshapeWindow( ScreenWidth, ScreenHeight );
	glViewport( 0, 0, ScreenWidth, ScreenHeight );


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
			// Calc/Open Julia Set
			if ( !IsJulia )
			{
				JuliaPoints = julia.GetPoints( JuliaSeed, 1000, 1000, 1000, CurrentScheme );
			}
			IsJulia = !IsJulia;
			break;
		case C:
			ChangeColor();
			SetPointColors();
			break;
		case R:
			GenerateRandomColorScheme();
			SetPointColors();
			break;
		case A:
			if ( !Animating )
			{
				CurrentScheme = ColorSchemes.at(0);
			}
			Animating = !Animating;
			break;
	}	
	glutPostRedisplay();
}

void mouseclick( int button, int state, int x, int y )
{
	
}

void mousemove( int x, int y )
{
	// invert y
	y = ScreenHeight - y;


	float xmin = 0;
	float xmax = 0;
	float ymin = 0;
	float ymax = 0;

	if ( !IsJulia ) 
	{
		// track the mouse position to open Julia Set
		xmin = mandelbrot.GetComplexXMin();
		xmax = mandelbrot.GetComplexXMax();
		ymin = mandelbrot.GetComplexYMin();
		ymax = mandelbrot.GetComplexYMax();


		float gridStepsX = ( ( fabs(xmax) + fabs(xmin) ) / ScreenWidth );
		float plotx = xmin + ( x * gridStepsX );

		float gridStepsY = ( ( fabs(ymax) + fabs(ymin) ) / ScreenHeight );
		float ploty = ymin + ( y * gridStepsY );

		JuliaSeed.x = plotx;
		JuliaSeed.y = ploty;
	}
	else 
	{
		// Not sure what to do here
		xmin = julia.GetComplexXMin();
		xmax = julia.GetComplexXMax();
		ymin = julia.GetComplexYMin();
		ymax = julia.GetComplexYMax();

		float gridStepsX = ( ( fabs(xmax) + fabs(xmin) ) / ScreenWidth );
		float gridStepsY = ( ( fabs(ymax) + fabs(ymin) ) / ScreenHeight );
		
		float plotx = xmin + ( x * gridStepsX );
		float ploty = ymin + ( y * gridStepsY );

		JuliaSeed.x = plotx;
		JuliaSeed.y = ploty;
	}
}

void update( int value )
{
	if ( Animating ) 
	{
		CurrentScheme.black = CurrentScheme.color10;

		CurrentScheme.color1 = CurrentScheme.color2;
		CurrentScheme.color2 = CurrentScheme.color3;
		CurrentScheme.color3 = CurrentScheme.color4;
		CurrentScheme.color4 = CurrentScheme.color5;
		CurrentScheme.color5 = CurrentScheme.color6;
		CurrentScheme.color6 = CurrentScheme.color7;
		CurrentScheme.color7 = CurrentScheme.color8;
		CurrentScheme.color8 = CurrentScheme.color9;
		CurrentScheme.color9 = CurrentScheme.color10;
		CurrentScheme.color10 = CurrentScheme.black;
		glutPostRedisplay();
	}
	SetPointColors();
	
	glutTimerFunc( 100, update, 0 );
}

void idle() 
{
	if ( Animating ) 
	{
		CurrentScheme.black = CurrentScheme.color10;

		CurrentScheme.color1 = CurrentScheme.color2;
		CurrentScheme.color2 = CurrentScheme.color3;
		CurrentScheme.color3 = CurrentScheme.color4;
		CurrentScheme.color4 = CurrentScheme.color5;
		CurrentScheme.color5 = CurrentScheme.color6;
		CurrentScheme.color6 = CurrentScheme.color7;
		CurrentScheme.color7 = CurrentScheme.color8;
		CurrentScheme.color8 = CurrentScheme.color9;
		CurrentScheme.color9 = CurrentScheme.color10;
		CurrentScheme.color10 = CurrentScheme.black;
		glutPostRedisplay();
	}
	SetPointColors();
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
	manWin = glutCreateWindow( "NineteenSixtyX" );
	
	// color used to clear screen - black
	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	// callbacks
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );
	glutPassiveMotionFunc( mousemove );
	//glutTimerFunc( 100, update, 0 );
	//glutIdleFunc( idle );
}

// TODO:
// Alternatively (for this method and SetPointColors) we could add a 
// GetColor( int index ) and SetColor( int index ) to a ColorScheme class
// where there is a swtich statement to determine which color would be 
// retrieved or set. Essentially would move code from main to elsewhere.
// ^ is probably the best choice
void GenerateRandomColorScheme()
{
	float r;
	float g;
	float b;

	ColorScheme randomScheme;
	for( int i = 0; i < 11; i++ ) 
	{
		r = (float)(rand()) / (float)(RAND_MAX);
		g = (float)(rand()) / (float)(RAND_MAX);
		b = (float)(rand()) / (float)(RAND_MAX);
		switch( i )
		{
			case 0:
				// TODO:
				// keep these black or change to random color??
				// black looks more right.
				randomScheme.black = Color( 0, 0, 0 );
				break;
			case 1:
				randomScheme.color1 = Color( r, g, b );
				break;
			case 2:
				randomScheme.color2 = Color( r, g, b );
				break;
			case 3:
				randomScheme.color3 = Color( r, g, b );
				break;
			case 4:
				randomScheme.color4 = Color( r, g, b );
				break;
			case 5:
				randomScheme.color5 = Color( r, g, b );
				break;
			case 6:
				randomScheme.color6 = Color( r, g, b );
				break;
			case 7:
				randomScheme.color7 = Color( r, g, b );
				break;
			case 8:
				randomScheme.color8 = Color( r, g, b );
				break;
			case 9:
				randomScheme.color9 = Color( r, g, b );
				break;
			case 10:
				randomScheme.color10 = Color( r, g, b );
				break;
		}
	}
	ColorSchemes.push_back( randomScheme );
	CurrentScheme = randomScheme;
}

void CreateColorVector() 
{
	ColorScheme scheme;
	scheme.black = Color( 0, 0, 0 );

	scheme.color1 = Color( 1, .5, 0 );
	scheme.color2 = Color( 1, 0, 0 );
	scheme.color3 = Color( 0, 0, .5 );
	scheme.color4 = Color( 1, 1, 0 );
	scheme.color5 = Color( 0, .3, 0 );
	scheme.color6 = Color( 0, .3, .3 );
	scheme.color7 = Color( 0, .5, .5 );
	scheme.color8 = Color( 0, .7, .7 );
	scheme.color9 = Color( 0, .9, .9 );
	scheme.color10 = Color( 0, 1, 1 );
	ColorSchemes.push_back( scheme );

	scheme.color1 = Color( 0, .5, 1 );
	scheme.color2 = Color( 0, 1, .7 );
	scheme.color3 = Color( .7, 0, 0);
	scheme.color4 = Color( .5, .5, 0 );
	scheme.color5 = Color( 1, 0, .5 );
	scheme.color6 = Color( .3, .3, .3 );
	scheme.color7 = Color( .5, .5, .3 );
	scheme.color8 = Color( .7, .7, .3 );
	scheme.color9 = Color( .9, .9, .3 );
	scheme.color10 = Color( 1, 1, 1 );
	ColorSchemes.push_back( scheme );
}

void ChangeColor() 
{
	int index = rand() % ColorSchemes.size();
	ColorScheme newScheme = ColorSchemes.at(index);
	
	// Make sure we don't get the same color
	while ( EqualSchemes( newScheme, CurrentScheme ) ) 
	{
		index = rand() % ColorSchemes.size();
		newScheme = ColorSchemes.at(index);
	}
	CurrentScheme = newScheme;
}

// Alternatively could make ColorScheme a class with an equals method.
// The alternative is probably better
bool EqualSchemes( ColorScheme scheme1, ColorScheme scheme2 )
{
	return scheme1.color1.equals( scheme2.color1 ) &&
		   scheme1.color2.equals( scheme2.color2 ) &&
		   scheme1.color3.equals( scheme2.color3 ) &&
		   scheme1.color4.equals( scheme2.color4 ) &&
		   scheme1.color5.equals( scheme2.color5 ) &&
		   scheme1.color6.equals( scheme2.color6 ) &&
		   scheme1.color7.equals( scheme2.color7 ) &&
		   scheme1.color8.equals( scheme2.color8 ) &&
		   scheme1.color9.equals( scheme2.color9 ) &&
		   scheme1.color10.equals( scheme2.color10 );
}

void SetPointColors()
{
	vector< ComplexPoint > &pointVec = IsJulia ? JuliaPoints : MandelbrotPoints;
	for( int i = 0; i < pointVec.size(); i++ ) 
	{
		ComplexPoint &pt = pointVec.at(i);
		switch( pt.schemeIndex )
		{
			case 0:
				pt.color = CurrentScheme.black;
				break;
			case 1:
				pt.color = CurrentScheme.color1;
				break;
			case 2:
				pt.color = CurrentScheme.color2;
				break;
			case 3:
				pt.color = CurrentScheme.color3;
				break;
			case 4:
				pt.color = CurrentScheme.color4;
				break;
			case 5:
				pt.color = CurrentScheme.color5;
				break;
			case 6:
				pt.color = CurrentScheme.color6;
				break;
			case 7:
				pt.color = CurrentScheme.color7;
				break;
			case 8:
				pt.color = CurrentScheme.color8;
				break;
			case 9:
				pt.color = CurrentScheme.color9;
				break;
			case 10:
				pt.color = CurrentScheme.color10;
				break;
		}
	}
}

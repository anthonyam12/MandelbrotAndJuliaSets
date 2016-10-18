#include "mandelbrot.h" 

int manWin, julWin;

int main( int argc, char* argv[] )
{
	srand( time( NULL ) );

	CreateColorVector();
	CurrentScheme = ColorSchemes.at(0);

	// get Mandelbrot points
	MandelbrotPoints = mandelbrot.GetPoints( 500, 500, 1000 );
	//vector<ComplexPoint> test = mandelbrotCu.GetPoints(1000, 1000, 1000, NULL);

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
	glLoadIdentity();

	MinMax mm = GetMinMax();	

	gluOrtho2D( mm.xmin, mm.xmax, mm.ymin, mm.ymax );
	glViewport( 0, 0, ScreenWidth, ScreenHeight );

	glClear( GL_COLOR_BUFFER_BIT );	
	glutSwapBuffers();

	vector<ComplexPoint> plotVec = IsJulia ? JuliaPoints : MandelbrotPoints;

	ComplexPoint pt;
	glBegin( GL_POINTS );
	for( int i = 0; i < plotVec.size();  i++ )
	{
		pt = plotVec.at(i);
		pt.color = CurrentScheme.GetColor( pt.schemeIndex );
		float color[3] = { pt.color.r, pt.color.g, pt.color.b };
		glColor3fv( color );
		glVertex2f( pt.x, pt.y );
	}	
	glEnd();

	glFlush();
}

void reshape( int w, int h )
{
	Reshape = !Reshape;
	if ( Reshape )
	{
		glutReshapeWindow( ScreenWidth, ScreenHeight );
	}

	MinMax mm = GetMinMax();

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	gluOrtho2D( mm.xmin, mm.xmax, mm.ymin, mm.ymax  );
	glViewport( 0, 0, ScreenWidth, ScreenHeight );

	glClear( GL_COLOR_BUFFER_BIT );
}

void keyboard( unsigned char key, int x, int y )
{
	MinMax mm = GetMinMax();
	double xlength = mm.xmax - mm.xmin;
	double ylength = mm.ymax - mm.ymin;

	// +/- keys for zoom 
	// J - toggle between Mandelbrot and Julia Sets at Current Cursor position
	// 	   (open new window for Julia)
	// C - change color maps
	// R - generate random color map
	// A - animate by cycling between color maps
	switch ( key ) 
	{
		case Plus:
			// TODO: there is probably a more efficient way to handle the IsJulia stuff
			if ( !IsJulia )
			{
				MandelbrotStepsX = MandelbrotStepsX > 1200 ? MandelbrotStepsX : MandelbrotStepsX*1.1;
				MandelbrotStepsY = MandelbrotStepsY > 1200 ? MandelbrotStepsY : MandelbrotStepsY*1.1;
				mandelbrot.SetComplexXMax( mm.xmax - ( xlength*.1 ) );
				mandelbrot.SetComplexXMin( mm.xmin + ( xlength*.1 ) );
				mandelbrot.SetComplexYMax( mm.ymax - ( ylength*.1 ) );
				mandelbrot.SetComplexYMin( mm.ymin + ( ylength*.1 ) );
				MandelbrotPoints = mandelbrot.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );
			}
			else 
			{
				JuliaStepsX = JuliaStepsX > 1200 ? JuliaStepsX : JuliaStepsX * 1.1;
				JuliaStepsY = JuliaStepsY > 1200 ? JuliaStepsY : JuliaStepsY * 1.1;
				julia.SetComplexXMax( mm.xmax - ( xlength*.1 ) );
				julia.SetComplexXMin( mm.xmin + ( xlength*.1 ) );
				julia.SetComplexYMax( mm.ymax - ( ylength*.1 ) );
				julia.SetComplexYMin( mm.ymin + ( ylength*.1 ) );
				JuliaPoints = julia.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
			}
			break;
		case Minus:
			// TODO: for zoom out we need to add on more than 10% because of how precents work, obvs
			if ( !IsJulia )
			{
				MandelbrotStepsX = MandelbrotStepsX < 500 
								   ? MandelbrotStepsX 
								   : MandelbrotStepsX/1.1;
				MandelbrotStepsY = MandelbrotStepsY < 500 
								   ? MandelbrotStepsY 
								   : MandelbrotStepsY/1.1;
				mandelbrot.SetComplexXMax( mm.xmax + ( xlength*.1 ) );
				mandelbrot.SetComplexXMin( mm.xmin - ( xlength*.1 ) );
				mandelbrot.SetComplexYMax( mm.ymax + ( ylength*.1 ) );
				mandelbrot.SetComplexYMin( mm.ymin - ( ylength*.1 ) );
				MandelbrotPoints = mandelbrot.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );
			}
			else 
			{
				JuliaStepsX = JuliaStepsX < 500 ? JuliaStepsX : JuliaStepsX / 1.1;
				JuliaStepsY = JuliaStepsY < 500 ? JuliaStepsY : JuliaStepsY / 1.1;
				julia.SetComplexXMax( mm.xmax + ( xlength*.1 ) );
				julia.SetComplexXMin( mm.xmin - ( xlength*.1 ) );
				julia.SetComplexYMax( mm.ymax + ( ylength*.1 ) );
				julia.SetComplexYMin( mm.ymin - ( ylength*.1 ) );
				JuliaPoints = julia.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
			}
			break;
		case J:
			// Calc/Open Julia Set
			julia.SetComplexXMax( 2 );
			julia.SetComplexXMin( -2 );
			julia.SetComplexYMax( 2 );
			julia.SetComplexYMin( -2 );
			if ( !IsJulia )
			{
				JuliaPoints = julia.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
			}
			JuliaStepsX = 500;
			JuliaStepsY = 500;
			IsJulia = !IsJulia;
			break;
		case C:
			ChangeColor();
			break;
		case R:
			GenerateRandomColorScheme();
			break;
		case A:
			Animating = !Animating;
			if ( !Animating )
			{
				CurrentScheme = BeforeAnimating;
			}
			else 
			{
				BeforeAnimating = CurrentScheme;
			}
			break;
		case EscapeKey:
			exit( 0 );
			break;
	}	
	glutPostRedisplay();
}

void special( int key, int x, int y )
{
	MinMax mm = GetMinMax();
	
	double xlength = mm.xmax - mm.xmin;
	double ylength = mm.ymax - mm.ymin;

	switch( key )
	{
		case GLUT_KEY_RIGHT:
			if ( !IsJulia )
			{
				mandelbrot.SetComplexXMin( mm.xmin + ( xlength*.3 ) );
				mandelbrot.SetComplexXMax( mm.xmax + ( xlength*.3 ) );			
				MandelbrotPoints = mandelbrot.GetPoints( 500, 500, 1000 );
			}
			else 
			{	
				julia.SetComplexXMin( mm.xmin + ( xlength*.3 ) );
				julia.SetComplexXMax( mm.xmax + ( xlength*.3 ) );			
				JuliaPoints = julia.GetPoints( JuliaSeed, 500, 500, 1000 );
			}
			break;
		case GLUT_KEY_LEFT:
			if ( !IsJulia )
			{
				mandelbrot.SetComplexXMin( mm.xmin - ( xlength*.3 ) );
				mandelbrot.SetComplexXMax( mm.xmax - ( xlength*.3 ) );
				MandelbrotPoints = mandelbrot.GetPoints( 500, 500, 1000 );
			} 
			else 
			{	
				julia.SetComplexXMin( mm.xmin - ( xlength*.3 ) );
				julia.SetComplexXMax( mm.xmax - ( xlength*.3 ) );
				JuliaPoints = julia.GetPoints( JuliaSeed, 500, 500, 1000 );
			}
			break;
		case GLUT_KEY_UP:
			if ( !IsJulia )
			{
				mandelbrot.SetComplexYMin( mm.ymin + ( ylength*.3 ) );
				mandelbrot.SetComplexYMax( mm.ymax + ( ylength*.3 ) );
				MandelbrotPoints = mandelbrot.GetPoints( 500, 500, 1000 );
			}
			else 
			{
				julia.SetComplexYMin( mm.ymin + ( ylength*.3 ) );
				julia.SetComplexYMax( mm.ymax + ( ylength*.3 ) );
				JuliaPoints = julia.GetPoints( JuliaSeed, 500, 500, 1000 );
			}
			break;
		case GLUT_KEY_DOWN:
			if ( !IsJulia )
			{
				mandelbrot.SetComplexYMin( mm.ymin - ( ylength*.3 ) );
				mandelbrot.SetComplexYMax( mm.ymax - ( ylength*.3 ) );
				MandelbrotPoints = mandelbrot.GetPoints( 500, 500, 1000 );
			}
			else 
			{
				julia.SetComplexYMin( mm.ymin - ( ylength*.3 ) );
				julia.SetComplexYMax( mm.ymax - ( ylength*.3 ) );
				JuliaPoints = julia.GetPoints( JuliaSeed, 500, 500, 1000 );
			}
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
	MinMax mm = GetMinMax();

	if ( !IsJulia ) 
	{
		// track the mouse position to open Julia Set
		double gridStepsX = ( ( fabs(mm.xmax) + fabs(mm.xmin) ) / ScreenWidth );
		double plotx = mm.xmin + ( x * gridStepsX );

		double gridStepsY = ( ( fabs(mm.ymax) + fabs(mm.ymin) ) / ScreenHeight );
		double ploty = mm.ymin + ( y * gridStepsY );

		JuliaSeed.x = plotx;
		JuliaSeed.y = ploty;
	}
	else 
	{
		
	}
	//cout << JuliaSeed.x << ", " << JuliaSeed.y << endl;
}

void update( int value )
{
	if( Animating )
	{
		ColorScheme constScheme = CurrentScheme;
		for( int i = 0; i < 11; i++ )
		{
			CurrentScheme.SetColor( i, constScheme.GetColor( ( i + 10 ) % 11 ) );
		}
		glutPostRedisplay();
	}
	glutTimerFunc( 200, update, 0 );
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
	glutSpecialFunc( special );

	glutTimerFunc( 200, update, 0 );
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
	Color black( 0, 0, 0 );
	randomScheme.SetColor( 0, black );

	// set colors 1 through 10
	for( int i = 1; i < 11; i++ ) 
	{
		r = (float)(rand()) / (float)(RAND_MAX);
		g = (float)(rand()) / (float)(RAND_MAX);
		b = (float)(rand()) / (float)(RAND_MAX);
		randomScheme.SetColor( i, Color( r, g, b ) );
	}

	ColorSchemes.push_back( randomScheme );
	CurrentScheme = randomScheme;
}

void CreateColorVector() 
{
	ColorScheme scheme;
	scheme.SetColor( 0, Color( 0, 0, 0 ) );

	scheme.SetColor( 1, Color( 1, .5, 0 ) );
	scheme.SetColor( 2, Color( 1, 0, 0 ) );
	scheme.SetColor( 3, Color( 0, 0, .5 ) );
	scheme.SetColor( 4, Color( 1, 1, 0 ) );
	scheme.SetColor( 5, Color( 0, .3, 0 ) );
	scheme.SetColor( 6, Color( 0, .3, .3 ) ) ;
	scheme.SetColor( 7, Color( 0, .5, .5 ) );
	scheme.SetColor( 8, Color( 0, .7, .7 ) ) ;
	scheme.SetColor( 9, Color( 0, .9, .9 ) );
	scheme.SetColor( 10, Color( 0, 1, 1 ) );
	ColorSchemes.push_back( scheme );

	scheme.SetColor( 1, Color( 0, .5, 1 ) );
	scheme.SetColor( 2, Color( 0, 1, .7 ) );
	scheme.SetColor( 3, Color( .7, 0, 0) );
	scheme.SetColor( 4, Color( .5, .5, 0 ) );
	scheme.SetColor( 5, Color( 1, 0, .5 ) );
	scheme.SetColor( 6, Color( .3, .3, .3 ) );
	scheme.SetColor( 7, Color( .5, .5, .3 ) );
	scheme.SetColor( 8, Color( .7, .7, .3 ) );
	scheme.SetColor( 9, Color( .9, .9, .3 ) );
	scheme.SetColor( 10, Color( 1, 1, 1 ) );
	ColorSchemes.push_back( scheme );
}

void ChangeColor() 
{
	int index = rand() % ColorSchemes.size();
	ColorScheme newScheme = ColorSchemes.at(index);
	
	// Make sure we don't get the same color
	while ( CurrentScheme.Equals( newScheme ) ) 
	{
		index = rand() % ColorSchemes.size();
		newScheme = ColorSchemes.at(index);
	}
	CurrentScheme = newScheme;
}

MinMax GetMinMax()
{
	double xmin = 0;
	double xmax = 0;
	double ymin = 0;
	double ymax = 0;

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

	MinMax minMax;

	minMax.xmin = xmin;
	minMax.xmax = xmax;
	minMax.ymin = ymin;
	minMax.ymax = ymax;

	return minMax;
}

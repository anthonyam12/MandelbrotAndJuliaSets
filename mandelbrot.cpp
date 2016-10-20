/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* Date: 10/21/2016
* 
* CSC - 533 Computer Graphics, Dr. John Weiss
* Program 2 - Mandelbrot and Julia Set Explorer
*
* Description:
*
* Core Features:
*
* Known Bugs:
*	- Sort of a bug: zoom in and out don't zoom the same amount. Would need to
*	  the math behind the operations.
*	- On the CPU, the zoom out makes lines appear because we are taking too 
*	  many steps away from nx and ny too quickly. That is, we would need to 
*	  decrease nx and ny more intelligently to prevent these lines. Not a 
*	  problem running on GPU 
*
* Potential Improvements:
*	- Could make the Julia and Mandelbrot sets implement an interface with the 
*	  get/set min/max values, this would elimate quite a few lines of code and
*	  many many checks throughout the program if the interface also contained 
*	  the GetPoints() method.
*	- Pass in 2D vectors to the GPU kernels so that each thread executes one point.
*	  As it is now, each kernel computer one column since we iterate the y values
*	  on each thread.
*	- Make the if statements in the Mandelbrot and Julia set color settings to use
*	  a part of MaxIters rather than hardcoded values.
*
*******************************************************************************/
#include "mandelbrot.h" 

/*******************************************************************************
* main() - control over the program, get's things started. 
*******************************************************************************/
int main( int argc, char* argv[] )
{
	srand( time( NULL ) );

	// get command line input (only care about '-cpu')
	if ( argc > 1 )
	{	
		string input = argv[1];
		if ( input == "-cpu" )
		{
			GPU = !GPU;
		}
	}

	// store two colors at the start
	CreateColorVector();
	// set the default color scheme
	CurrentScheme = ColorSchemes.at(0);

	// get Mandelbrot points
	if ( !GPU )
	{
		MandelbrotPoints = mandelbrot.GetPoints( 1000, 1000, 1000 );
	}
	else 
	{
		MandelbrotPoints = mandelbrotCu.GetPoints(1000, 1000, 1000);
	}
	
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
*******************************************************************************/

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief Displays the Mandelbrot or Julia Points
*
* An OpenGL callback to display the sets in the Mandelbrot or Julia sets. 
*
* \params none
* \return none
*******************************************************************************/
void display( void )
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	// get xmin, xmax, ymin and ymax in struct form
	MinMax mm = GetMinMax();

	// set the viewpost and projection
	gluOrtho2D( mm.xmin, mm.xmax, mm.ymin, mm.ymax );
	glViewport( 0, 0, ScreenWidth, ScreenHeight );

	glClear( GL_COLOR_BUFFER_BIT );

	// determine if we're drawing a Julia set or the Mandelbrot set
	vector<ComplexPoint> plotVec = IsJulia ? JuliaPoints : MandelbrotPoints;

	// draw the points in their scheme color 
	ComplexPoint pt;
	glBegin( GL_POINTS );
	for( int i = 0; i < plotVec.size();  i++ )
	{
		pt = plotVec.at(i);
		// get the color from the scheme
		Color color = CurrentScheme.GetColor( pt.schemeIndex );
		float colorv[3] = { color.r, color.g, color.b };
		glColor3fv( colorv );
		glVertex2f( pt.x, pt.y );
	}	
	glEnd();

	glutSwapBuffers();
}

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief The OpenGL callback function called when resizing the window.
*
* In this particular case resizing the window is prevented. This is to keep the
* computations down when running on the CPU. If the window is enlarged there 
* would need to be more steps in the x and y directions. Furthermore, it would
* be wise to keep w = h since the distortion really affects the sets. To avoid
* all of this we chose to prevent resizing.
*
* \params w - screen width
*		  h - screen height
* \return none
*******************************************************************************/
void reshape( int w, int h )
{
	// want to call this reshape function to set the screen back to the original
	// size (prevents resizing). If we don't have the flag it is an infinite loop
	// since glutReshapeWindow() calls reshape().
	Reshape = !Reshape;
	if ( Reshape )
	{
		glutReshapeWindow( ScreenWidth, ScreenHeight );
	}

	// get x/y min/max 
	MinMax mm = GetMinMax();

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	// set viewport and projection coords
	gluOrtho2D( mm.xmin, mm.xmax, mm.ymin, mm.ymax  );
	glViewport( 0, 0, ScreenWidth, ScreenHeight );

	glClear( GL_COLOR_BUFFER_BIT );
}

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief 
*
* \params
* \return
*******************************************************************************/
void keyboard( unsigned char key, int x, int y )
{
	// +/- keys for zoom 
	// J - toggle between Mandelbrot and Julia Sets at Current Cursor position
	// 	   (open new window for Julia)
	// C - change color maps
	// R - generate random color map
	// A - animate by cycling between color maps
	switch ( key ) 
	{
		case Plus:
			zoom( true );
			break;
		case Minus:
			zoom( false );
			break;
		case J:
			// Calc/Open Julia Set
			julia.SetComplexXMax( 2 );
			julia.SetComplexXMin( -2 );
			julia.SetComplexYMax( 2 );
			julia.SetComplexYMin( -2 );
			if ( !IsJulia )
			{
				if ( !GPU )
				{
					JuliaPoints = julia.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
				}
				else
				{
					JuliaPoints = juliaCu.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
				}
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

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief 
*
* \params
* \return
*******************************************************************************/
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
				if ( !GPU )
				{
					mandelbrot.SetComplexXMin( mm.xmin + ( xlength*.3 ) );
					mandelbrot.SetComplexXMax( mm.xmax + ( xlength*.3 ) );			
					MandelbrotPoints = mandelbrot.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );
				}
				else 
				{
					mandelbrotCu.SetComplexXMin( mm.xmin + ( xlength*.3 ) );
					mandelbrotCu.SetComplexXMax( mm.xmax + ( xlength*.3 ) );			
					MandelbrotPoints = mandelbrotCu.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );
				}
			}
			else 
			{	
				if( !GPU ) 
				{
					julia.SetComplexXMin( mm.xmin + ( xlength*.3 ) );
					julia.SetComplexXMax( mm.xmax + ( xlength*.3 ) );			
					JuliaPoints = julia.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
				}
				else 
				{
					juliaCu.SetComplexXMin( mm.xmin + ( xlength*.3 ) );
					juliaCu.SetComplexXMax( mm.xmax + ( xlength*.3 ) );			
					JuliaPoints = juliaCu.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
				}
			}
			break;
		case GLUT_KEY_LEFT:
			if ( !IsJulia )
			{
				if ( !GPU )
				{
					mandelbrot.SetComplexXMin( mm.xmin - ( xlength*.3 ) );
					mandelbrot.SetComplexXMax( mm.xmax - ( xlength*.3 ) );
					MandelbrotPoints = mandelbrot.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );
				}
				else 
				{
					mandelbrotCu.SetComplexXMin( mm.xmin - ( xlength*.3 ) );
					mandelbrotCu.SetComplexXMax( mm.xmax - ( xlength*.3 ) );
					MandelbrotPoints = mandelbrotCu.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );
				}
			} 
			else 
			{	
				if ( !GPU )
				{
					julia.SetComplexXMin( mm.xmin - ( xlength*.3 ) );
					julia.SetComplexXMax( mm.xmax - ( xlength*.3 ) );
					JuliaPoints = julia.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
				}
				else 
				{
					juliaCu.SetComplexXMin( mm.xmin - ( xlength*.3 ) );
					juliaCu.SetComplexXMax( mm.xmax - ( xlength*.3 ) );
					JuliaPoints = juliaCu.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
				}
			}
			break;
		case GLUT_KEY_UP:
			if ( !IsJulia )
			{
				if ( !GPU )
				{
					mandelbrot.SetComplexYMin( mm.ymin + ( ylength*.3 ) );
					mandelbrot.SetComplexYMax( mm.ymax + ( ylength*.3 ) );
					MandelbrotPoints = mandelbrot.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );
				}
				else 
				{
					mandelbrotCu.SetComplexYMin( mm.ymin + ( ylength*.3 ) );
					mandelbrotCu.SetComplexYMax( mm.ymax + ( ylength*.3 ) );
					MandelbrotPoints = mandelbrotCu.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );
				}
			}
			else 
			{
				if ( !GPU )
				{
					julia.SetComplexYMin( mm.ymin + ( ylength*.3 ) );
					julia.SetComplexYMax( mm.ymax + ( ylength*.3 ) );
					JuliaPoints = julia.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
				}
				else 
				{
					juliaCu.SetComplexYMin( mm.ymin + ( ylength*.3 ) );
					juliaCu.SetComplexYMax( mm.ymax + ( ylength*.3 ) );
					JuliaPoints = juliaCu.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
				}
			}
			break;
		case GLUT_KEY_DOWN:
			if ( !IsJulia )
			{
				if ( !GPU )
				{
					mandelbrot.SetComplexYMin( mm.ymin - ( ylength*.3 ) );
					mandelbrot.SetComplexYMax( mm.ymax - ( ylength*.3 ) );
					MandelbrotPoints = mandelbrot.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );
				}
				else 
				{
					mandelbrotCu.SetComplexYMin( mm.ymin - ( ylength*.3 ) );
					mandelbrotCu.SetComplexYMax( mm.ymax - ( ylength*.3 ) );
					MandelbrotPoints = mandelbrotCu.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );
				}
			}
			else 
			{	
				if ( !GPU )
				{
					julia.SetComplexYMin( mm.ymin - ( ylength*.3 ) );
					julia.SetComplexYMax( mm.ymax - ( ylength*.3 ) );
					JuliaPoints = julia.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
				}
				else 
				{
					juliaCu.SetComplexYMin( mm.ymin - ( ylength*.3 ) );
					juliaCu.SetComplexYMax( mm.ymax - ( ylength*.3 ) );
					JuliaPoints = juliaCu.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
				}
			}
			break;
	}

	glutPostRedisplay();
}

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief 
*
* \params
* \return
*******************************************************************************/
void mouseclick( int button, int state, int x, int y )
{
	if( button == 3 )
	{
		zoom( true );	
	}
	if( button == 4 )
	{
		zoom( false );
	}
	glutPostRedisplay();
}

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief 
*
* \params
* \return
*******************************************************************************/
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
}

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief Animates the colors in the currently displayed set
*
* Due to how our points get their colors, in order to animate the colors we only
* need to change the color's index within it's color scheme. A points color is
* set right before it is drawn by calling into the color scheme class to get a
* color at a specific index (0-10). This function changes the indexes of the 
* colors so they appear to be moving outwards from the center of the set.
*
* \params value - not used
* \return none
*******************************************************************************/
void update( int value )
{
	// if we are animating we want to change things, otherwise don't
	if( Animating )
	{
		// make a copy of the current scheme so we can use the colors after 
		// they change
		ColorScheme constScheme = CurrentScheme;
		for( int i = 0; i < 11; i++ )
		{
			// change the colors to the 'next' color
			CurrentScheme.SetColor( i, constScheme.GetColor( ( i + 10 ) % 11 ) );
		}
		glutPostRedisplay();
	}
	glutTimerFunc( 100, update, 0 );
}

/*******************************************************************************
 *								Misc. Functions								   *
 ******************************************************************************/
/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief Initializes OpenGL states and callbacks
*
* Initializes OpenGL states and callbacks. Sets the window size and title.
* Makes the initial call to our update function for animation.
*
* \params none
* \return none
*******************************************************************************/
void initOpenGL( void )
{
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
	
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
	glutMouseFunc( mouseclick );
	glutSpecialFunc( special );

	// update function for animation
	glutTimerFunc( 100, update, 0 );
}

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief Generates a random color scheme
*
* Creates a new ColorScheme object with random colors for color 1 through color
* 10. However, we chose to keep the black points black in all schemes so that 
* color is not randomized. The random scheme becomes the current color scheme 
* and the random scheme is added to a vector containing all color schemes.
*
* \params none 
* \return none
*******************************************************************************/
void GenerateRandomColorScheme()
{
	float r;
	float g;
	float b;

	// create new scheme and set black to the color black
	ColorScheme randomScheme;
	Color black( 0, 0, 0 );
	randomScheme.SetColor( 0, black );

	// set colors 1 through 10
	for( int i = 1; i < 11; i++ ) 
	{
		// get random red, green, and blue values
		r = (float)(rand()) / (float)(RAND_MAX);
		g = (float)(rand()) / (float)(RAND_MAX);
		b = (float)(rand()) / (float)(RAND_MAX);
		randomScheme.SetColor( i, Color( r, g, b ) );
	}

	// add the scheme to the vector of schemes
	ColorSchemes.push_back( randomScheme );
	// set the scheme to the current color scheme
	CurrentScheme = randomScheme;
}

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief Creates the default color scheme vector
*
* Creates two ColorScheme objects with two different color scheme values. These
* schemes are added to a global vector to store all color schemes. 
*
* \params none
* \return none
*******************************************************************************/
void CreateColorVector() 
{
	ColorScheme scheme;
	// we're going to keep the black points black, regardless of the other
	// scheme colors
	scheme.SetColor( 0, Color( 0, 0, 0 ) );

	// create and add a default scheme to the scheme vector
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

	// create and add a secondary scheme to the scheme vector 
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

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief Changes the color scheme of the set's points
*
* Selects a random color scheme from the color scheme vector and, if the scheme
* is currently not being used, the current scheme is set to the new scheme.
*
* \params none
* \return none
*******************************************************************************/
void ChangeColor() 
{
	// random scheme index
	int index = rand() % ColorSchemes.size();
	ColorScheme newScheme = ColorSchemes.at(index);
	
	// Make sure we don't get the same color
	while ( CurrentScheme.Equals( newScheme ) ) 
	{
		index = rand() % ColorSchemes.size();
		newScheme = ColorSchemes.at(index);
	}
	// set the scheme
	CurrentScheme = newScheme;
}

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief Returns a MinMax structure which stores x/y min/max values
*
* Determines if we need to get the min and max values from Julia sets of from the
* Mandelbrot set. Then using the GPU/CPU flag determines if the min and max
* values will come from the CPU or GPU versions of the classes used to get the
* points for these sets.
*
* \params none
* \return MinMax - a struct containing min/max information for the x and y values
*******************************************************************************/
MinMax GetMinMax()
{
	// struct to be returned
	MinMax mm;
	
	// determine which class to get the points from and call the proper getters
	if( !IsJulia )
	{
		if ( !GPU ) 
		{
			mm.xmin = mandelbrot.GetComplexXMin();
			mm.xmax = mandelbrot.GetComplexXMax();
			mm.ymin = mandelbrot.GetComplexYMin();
			mm.ymax = mandelbrot.GetComplexYMax();	
		}
		else 
		{
			mm.xmin = mandelbrotCu.GetComplexXMin();
			mm.xmax = mandelbrotCu.GetComplexXMax();
			mm.ymin = mandelbrotCu.GetComplexYMin();
			mm.ymax = mandelbrotCu.GetComplexYMax();	
		}
	}
	else 
	{
		if ( !GPU )
		{
			mm.xmin = julia.GetComplexXMin();
			mm.xmax = julia.GetComplexXMax();
			mm.ymin = julia.GetComplexYMin();
			mm.ymax = julia.GetComplexYMax();
		}
		else
		{
			mm.xmin = juliaCu.GetComplexXMin();
			mm.xmax = juliaCu.GetComplexXMax();
			mm.ymin = juliaCu.GetComplexYMin();
			mm.ymax = juliaCu.GetComplexYMax();
		}
	}

	return mm;
}

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief 
*
* \params
* \return
*******************************************************************************/
void zoom( bool zoomIn )
{
	MinMax mm = GetMinMax();
	double xlength = mm.xmax - mm.xmin;
	double ylength = mm.ymax - mm.ymin;

	if( zoomIn )
	{
		if ( !IsJulia )
		{
			MandelbrotStepsX = MandelbrotStepsX > 1200 ? MandelbrotStepsX : MandelbrotStepsX*1.1;
			MandelbrotStepsY = MandelbrotStepsY > 1200 ? MandelbrotStepsY : MandelbrotStepsY*1.1;
			if ( !GPU )
			{
				mandelbrot.SetComplexXMax( mm.xmax - ( xlength*.1 ) );
				mandelbrot.SetComplexXMin( mm.xmin + ( xlength*.1 ) );
				mandelbrot.SetComplexYMax( mm.ymax - ( ylength*.1 ) );
				mandelbrot.SetComplexYMin( mm.ymin + ( ylength*.1 ) );
				MandelbrotPoints = mandelbrot.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );	
			}
			else 
			{
				mandelbrotCu.SetComplexXMax( mm.xmax - ( xlength*.1 ) );
				mandelbrotCu.SetComplexXMin( mm.xmin + ( xlength*.1 ) );
				mandelbrotCu.SetComplexYMax( mm.ymax - ( ylength*.1 ) );
				mandelbrotCu.SetComplexYMin( mm.ymin + ( ylength*.1 ) );
				MandelbrotPoints = mandelbrotCu.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );	
			}
		}
		else 
		{
			JuliaStepsX = JuliaStepsX > 1200 ? JuliaStepsX : JuliaStepsX * 1.1;
			JuliaStepsY = JuliaStepsY > 1200 ? JuliaStepsY : JuliaStepsY * 1.1;
			if ( !GPU )
			{	
				julia.SetComplexXMax( mm.xmax - ( xlength*.1 ) );
				julia.SetComplexXMin( mm.xmin + ( xlength*.1 ) );
				julia.SetComplexYMax( mm.ymax - ( ylength*.1 ) );
				julia.SetComplexYMin( mm.ymin + ( ylength*.1 ) );
				JuliaPoints = julia.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
			}
			else 
			{
				juliaCu.SetComplexXMax( mm.xmax - ( xlength*.1 ) );
				juliaCu.SetComplexXMin( mm.xmin + ( xlength*.1 ) );
				juliaCu.SetComplexYMax( mm.ymax - ( ylength*.1 ) );
				juliaCu.SetComplexYMin( mm.ymin + ( ylength*.1 ) );
				JuliaPoints = juliaCu.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
			}
		}
	}
	else 
	{
		if ( !IsJulia )
		{
			MandelbrotStepsX = MandelbrotStepsX < 500 
							   ? MandelbrotStepsX 
							   : MandelbrotStepsX/1.1;
			MandelbrotStepsX = GPU ? 1200 : MandelbrotStepsX;
			MandelbrotStepsY = MandelbrotStepsY < 500 
							   ? MandelbrotStepsY 
							   : MandelbrotStepsY/1.1;
			MandelbrotStepsY = GPU ? 1200 : MandelbrotStepsY;
			if( !GPU )
			{
				mandelbrot.SetComplexXMax( mm.xmax + ( xlength*.1 ) );
				mandelbrot.SetComplexXMin( mm.xmin - ( xlength*.1 ) );
				mandelbrot.SetComplexYMax( mm.ymax + ( ylength*.1 ) );
				mandelbrot.SetComplexYMin( mm.ymin - ( ylength*.1 ) );
				MandelbrotPoints = mandelbrot.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );
			}
			else 
			{
				mandelbrotCu.SetComplexXMax( mm.xmax + ( xlength*.1 ) );
				mandelbrotCu.SetComplexXMin( mm.xmin - ( xlength*.1 ) );
				mandelbrotCu.SetComplexYMax( mm.ymax + ( ylength*.1 ) );
				mandelbrotCu.SetComplexYMin( mm.ymin - ( ylength*.1 ) );
				MandelbrotPoints = mandelbrotCu.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );
			}
		}
		else 
		{
			JuliaStepsX = JuliaStepsX < 500 ? JuliaStepsX : JuliaStepsX / 1.1;
			JuliaStepsX = GPU ? 1200 : JuliaStepsX;
			JuliaStepsY = JuliaStepsY < 500 ? JuliaStepsY : JuliaStepsY / 1.1;
			JuliaStepsY = GPU ? 1200 : JuliaStepsY;
			if( !GPU )
			{
				julia.SetComplexXMax( mm.xmax + ( xlength*.1 ) );
				julia.SetComplexXMin( mm.xmin - ( xlength*.1 ) );
				julia.SetComplexYMax( mm.ymax + ( ylength*.1 ) );
				julia.SetComplexYMin( mm.ymin - ( ylength*.1 ) );
				JuliaPoints = julia.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
			}
			else 
			{
				juliaCu.SetComplexXMax( mm.xmax + ( xlength*.1 ) );
				juliaCu.SetComplexXMin( mm.xmin - ( xlength*.1 ) );
				juliaCu.SetComplexYMax( mm.ymax + ( ylength*.1 ) );
				juliaCu.SetComplexYMin( mm.ymin - ( ylength*.1 ) );
				JuliaPoints = juliaCu.GetPoints( JuliaSeed, JuliaStepsX, JuliaStepsY, 1000 );
			}
		}
	}
}

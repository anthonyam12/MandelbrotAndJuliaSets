/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* Date: 10/21/2016
* 
* CSC - 533 Computer Graphics, Dr. John Weiss
* Program 2 - Mandelbrot and Julia Set Explorer
*
* Usage:
* 	mandelbrot 
* 	OR
* 	mandelbrot -cpu
*
* Description:
* 	This program is an explorer for Mandlebrot and Julia sets. The explorer
* allows zooming in and out on the Julia or Mandelbrot sets, changing the
* colors of the sets, generating a new color scheme for the set, and 
* animation of the fractals by cycling the color schemes. Additionally the 
* computation of the Mandelbrot and Julia sets is handled both on the GPU and on
* the CPU depending on a command line flag. If the '-cpu' is passed as a command
* line argument the computation of the set's points are run on the CPU, otherwise
* the computation is done by a GPU with CUDA cores. 
*
* 	The program incorporates a few classes to allow better abstraction and less
* code reuse. Those being, 
* 	1. Mandelbrot_points.cpp - a class used moslty for getting Mandlebrot set points
* 						on the GPU
*  	2. Mandelbrot_cuda.cu - same as Mandelbrot.cpp except the computations are
*  						performed on the GPU
*  	3. Julia.cpp - Julia set computations on the CPU
*  	4. Julia_cuda.cpp - Julia set computations on the GPU
*  	5. colorscheme.cpp - stores 11 colors and some functionality to operate on 
*  						the colors
*  	6. color.cpp - stores the R, G, B values of a color and operations on these
* 
* 	Contol of the program is as follows, 
* 		Panning - left, right, up and down arrow keys are used to pan left, 
* 					right, up, and down
* 		Zooming - zooming can be accomplished with the + and - keys or the mouse
* 					scroll wheel
* 		Animating - pressing the 'a' key will animate the colors
* 		Changing Colors - pressing 'c' will change the color scheme
* 		Julia Set Toggle - 'j' will toggle between Julia and Mandelbrot sets
*		Random Color - 'r' will generate a randome color scheme and store it in 
*					a vector. When changing colors the color is selected from 
*					this color scheme vector.
*
* Runtime:
*	NOTE: All numbers recieved on Opp Lab Computers
*	   Zoom on GPU: Average time = 0.33113275 (52 data points)
*	   Zoom on CPU: Average time = 2.01529092 (52 data points)
*	   GPU Speedup: 6.08605135x faster
*	   -------------------------------------------------------
*	   Pan on CPU:  Average time = 0.55548346 (52 data points)
*	   Pan on CPU:  Average time = 2.80379631 (52 data points)
*	   GPU Speedup: 5.04748838x faster
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
			// run on CPU rather than GPU
			GPU = !GPU;
		}
	}

	// store two colors at the start
	CreateColorVector();
	// set the default color scheme
	CurrentScheme = ColorSchemes.at(0);

	// get Mandelbrot points from CPU class or GPU class depending on flag
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
* \brief OpenGL callback to handle keyboard input 
*
* We only care about a few key presses, 
* 	J - toggles between Julia and Mandelbrot sets
* 	+/- - used for zooming in and out, respectively
* 	C - changes the color map
* 	R - generates a random color map and stores it
* 	A - animates the color map by cycling the colors
*	ESC - exits the program
* The method will determine which key was pressed and perform the operations 
* necessary. 
*
* \params key - key pressed
* 		  x - x position of mouse when key pressed
* 		  y - y position of mouse when key pressed
* \return none
*******************************************************************************/
void keyboard( unsigned char key, int x, int y )
{
	// +/- keys for zoom 
	// J - toggle between Mandelbrot and Julia Sets at Current Cursor position
	// 	   (open new window for Julia)
	// C - change color maps
	// R - generate random color map
	// A - animate by cycling colors in a color scheme
	switch ( key ) 
	{
		case Plus:
			// zoom in
			zoom( true );
			break;
		case Minus:
			// zoom out
			zoom( false );
			break;
		case J:
			// Calc/Open Julia Set
			// reset Julia x/y min/max
			julia.SetComplexXMax( 2 );
			julia.SetComplexXMin( -2 );
			julia.SetComplexYMax( 2 );
			julia.SetComplexYMin( -2 );
			// get julia points
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
			// reset number of steps
			JuliaStepsX = 500;
			JuliaStepsY = 500;
			// set a flag so the rest of the program acts accordingly
			IsJulia = !IsJulia;
			break;
		case C:
			// call a method to change the color scheme
			ChangeColor();
			// If we're animating and stop we want to go to the new
			// changed scheme rather than the scheme we were using
			// when we started animating.
			BeforeAnimating = CurrentScheme;
			break;
		case R:
			// generates a new color scheme and stores it on our vector
			// of schemes
			GenerateRandomColorScheme();
			// if we are animating and stop set scheme to new scheme
			BeforeAnimating = CurrentScheme;
			break;
		case A:
			// set animating flag for program to act accordingly
			Animating = !Animating;
			if ( !Animating )
			{
				// when we stop animating reset to the original color scheme
				CurrentScheme = BeforeAnimating;
			}
			else 
			{
				// set the scheme we want to revert back to when done animating
				BeforeAnimating = CurrentScheme;
			}
			break;
		case EscapeKey:
			// exit the program
			exit( 0 );
			break;
	}	
	glutPostRedisplay();
}

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief OpenGL callback to handle the use of the arrow keys
*
* In this program we choose to use the arrow keys to pan left, right, up, and
* down in the set. This callback will determine which key was pressed and will
* change the position of our set's midpoint accordingly. That is, if we pan up
* the midpoint is shifted up by a set amount by adding to the ymax and subtracting
* from the ymin. To pan right we add to xmax and subtract from xmin. Similar 
* operations are performed for left and down panning. 
*
* \params key - ID of key pressed
* 		  x - x position of activity
* 		  y - y position of activity
* \return none
*******************************************************************************/
void special( int key, int x, int y )
{
	// get the xmin, xmax, ymin, and ymax
	MinMax mm = GetMinMax();
	
	// will be working with the length of the x, y lines since our orthographic
	// midpoint is not at 0,0 in the viewport 
	double xlength = mm.xmax - mm.xmin;
	double ylength = mm.ymax - mm.ymin;

	switch( key )
	{
		// pan right
		case GLUT_KEY_RIGHT:
			if ( !IsJulia )
			{
				// handle Mandelbrot on GPU or CPU
				if ( !GPU )
				{
					// decrease xmin by adding 30% of the line length and increase
					// xmax by doing the same, the recalculate
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
				// Handle Julia CPU and GPU
				if( !GPU ) 
				{
					// same as above add 30% to the min/max and redraw
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
					// like above except we subtract from min and max to move left
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
					// similar to panning right and left except we modify the y values
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
					// decrease y values to pan down.
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
* \brief Zoom in and out with the mouse wheel
*
* Tracks movement of the mouse wheel to zoom in and out. 
*
* \params button - mouse button 'pressed'
* 		  state - pressed/released
* 		  x - x position of activity
* 		  y - y position of activity
* \return none
*******************************************************************************/
void mouseclick( int button, int state, int x, int y )
{
	// button 3 - mouse wheel up (zoom in)
	if( button == 3 )
	{
		zoom( true );	
	}
	// button 4 = mouse wheel down (zoom out)
	if( button == 4 )
	{
		zoom( false );
	}
	glutPostRedisplay();
}

/*******************************************************************************
* Authors: Anthony Morast, Samuel Carroll
* \brief Tracks the position of the mouse to set the seed for our Julia set
*
* Tracks the x,y position of the mouse which is used as a seed for the Julia set.
* Inverts y since down should mean down. Converts our points from the screen coords
* given by our viewport to the coords given by our orthographic projection.
* Sets the JuliaSeed x and y values to wherever the mouse is.
*
* \params int x - x position of the mouse in viewport coords
* 		  int y - y position of the mouse in viewport coords
* \return none
*******************************************************************************/
void mousemove( int x, int y )
{
	// invert y
	y = ScreenHeight - y;
	MinMax mm = GetMinMax();

	if ( !IsJulia ) 
	{
		// track the mouse position to open Julia Set
		// converts the viewport coords to our orthographic coords
		double gridStepsX = ( ( fabs(mm.xmax) + fabs(mm.xmin) ) / ScreenWidth );
		double plotx = mm.xmin + ( x * gridStepsX );

		double gridStepsY = ( ( fabs(mm.ymax) + fabs(mm.ymin) ) / ScreenHeight );
		double ploty = mm.ymin + ( y * gridStepsY );

		// set the Julia Seeds x and y values 
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
		CurrentScheme.SetColor( 1, constScheme.GetColor(10) );
		for( int i = 2; i < 11; i++ )
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
		// calls getters from CPU Mandelbrot
		if ( !GPU ) 
		{
			mm.xmin = mandelbrot.GetComplexXMin();
			mm.xmax = mandelbrot.GetComplexXMax();
			mm.ymin = mandelbrot.GetComplexYMin();
			mm.ymax = mandelbrot.GetComplexYMax();	
		}
		else 
		{
			// calls getters from Mandelbrot GPU
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
			// gets mins and maxs from julia CPU
			mm.xmin = julia.GetComplexXMin();
			mm.xmax = julia.GetComplexXMax();
			mm.ymin = julia.GetComplexYMin();
			mm.ymax = julia.GetComplexYMax();
		}
		else
		{
			// gets mins amd maxs from Julia GPU
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
* \brief Handles zooming in and out on the Mandelbrot/Julia sets
*
* This method starts by getting the min amd mac values for whichever class we 
* are dealing with (Mandelbrot CPU, Mandelbrot GPU, Julia CPU, or Julia GPU).
* It then, essentially, reduces or increases the X and Y min and maxes of the 
* current class and recalculates the points in our set. 
*
* \params zoomIn - bool to determine if we are zooming in or out
* \return none
*******************************************************************************/
void zoom( bool zoomIn )
{
	// x and y min and max
	MinMax mm = GetMinMax();
	// gets the x and y line length (keeps scale and midpoint by reducing by a 
	// factor of the total length of our x and y lines)
	double xlength = mm.xmax - mm.xmin;
	double ylength = mm.ymax - mm.ymin;

	if( zoomIn )
	{
		if ( !IsJulia )
		{
			// increase the number of steps as we zoom in
			MandelbrotStepsX = MandelbrotStepsX > 1200 ? MandelbrotStepsX : MandelbrotStepsX*1.1;
			MandelbrotStepsY = MandelbrotStepsY > 1200 ? MandelbrotStepsY : MandelbrotStepsY*1.1;
			// run CPU calcs
			if ( !GPU )
			{
				// reduce x and y mins and maxs to give us a 'zoomed in' set of points
				mandelbrot.SetComplexXMax( mm.xmax - ( xlength*.1 ) );
				mandelbrot.SetComplexXMin( mm.xmin + ( xlength*.1 ) );
				mandelbrot.SetComplexYMax( mm.ymax - ( ylength*.1 ) );
				mandelbrot.SetComplexYMin( mm.ymin + ( ylength*.1 ) );
				// re-calculate points
				MandelbrotPoints = mandelbrot.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );	
			}
			else 
			{
				// same as above except on the GPU
				mandelbrotCu.SetComplexXMax( mm.xmax - ( xlength*.1 ) );
				mandelbrotCu.SetComplexXMin( mm.xmin + ( xlength*.1 ) );
				mandelbrotCu.SetComplexYMax( mm.ymax - ( ylength*.1 ) );
				mandelbrotCu.SetComplexYMin( mm.ymin + ( ylength*.1 ) );
				MandelbrotPoints = mandelbrotCu.GetPoints( MandelbrotStepsX, MandelbrotStepsY, 1000 );	
			}
		}
		else 
		{
			// Same as above except with our Julia points on the CPU or GPU
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
		// zoom out
		if ( !IsJulia )
		{
			// if we're on the CPU we want to reduce the number of x, y steps for less computation
			MandelbrotStepsX = MandelbrotStepsX < 500 
							   ? MandelbrotStepsX 
							   : MandelbrotStepsX/1.1;
			// on GPU we don't really care so we keep it relatively high
			MandelbrotStepsX = GPU ? 1200 : MandelbrotStepsX;
			MandelbrotStepsY = MandelbrotStepsY < 500 
							   ? MandelbrotStepsY 
							   : MandelbrotStepsY/1.1;
			MandelbrotStepsY = GPU ? 1200 : MandelbrotStepsY;
			// increase the length of the x/y axis lenghts to give us more points (zoom out) 
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
			// same as above except on our Julia sets
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

#include "mandelbrot.h" 

int main( int argc, char* argv[] )
{
	
	return 0;
}

/*******************************************************************************
 *								OpenGL Functions							   *
 ******************************************************************************/
void dispaly( void )
{

}

void reshape( int w, int h )
{

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

void mousedrag( int x, int y )
{

}

/*******************************************************************************
 *								Misc. Functions								   *
 ******************************************************************************/
void initOpenGl( void )
{

}

float mandelbrot( int x, int y )
{
	return 0f;
}

float julia( int x, int y )
{
	return 0f;
}

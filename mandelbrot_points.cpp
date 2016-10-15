#include "global.h"


Mandelbrot::Mandelbrot()
{
		this->ComplexXMin = -2.00;
		this->ComplexXMax = .5;
		this->ComplexYMin = -1.25;
		this->ComplexYMax = 1.25;

		this->ComplexWidth = ComplexXMax - ComplexXMin;
		this->ComplexHeight = ComplexYMax - ComplexYMin;
}

vector< ComplexPoint > Mandelbrot::GetPoints( int nx, int ny,  int maxIter, Color colorScheme[7] ) 
{
	//TODO: Get the colors for the points from colorScheme[i] rather than setting to the book color
	//TODO: for animation we will go through our stored colored schemes and set the color in the 
	//TODO: MandelbrotPoints vector rather than calling this metod. As a matter of fact, it might
	//TODO: be best change the Color in the point struct to an int colorIndex; which would be an 
	//TODO: index to the color scheme we are currently using (values 0-6). Then we would just reset
	//TODO: a global colorScheme[7] array's values to the next color scheme and redraw the points.
	//TODO: We wouldn't need to change the points' colors at all since it is just an index to our 
	//TODO: current color scheme.
	vector< ComplexPoint > points;
	Color ptColor;

	ComplexPoint z, zIncr;
	int count = 0;

	zIncr.x = ComplexWidth / float( nx );
	zIncr.y = ComplexHeight / float( ny );

	for( z.x = ComplexXMin; z.x < ComplexXMax; z.x += zIncr.x )
	{
		for( z.y = ComplexYMin; z.y < ComplexYMax; z.y += zIncr.y ) 
		{
			//TODO: should we use more subdivisions for more definition?
			count = MandelbrotSqTransf( z, maxIter );
			if( count >= maxIter )
			{
				ptColor.r = ptColor.g = ptColor.b = 0.0;
			}
			else if ( count > ( maxIter / 8) ) 
			{
				ptColor.r = 1.0;
				ptColor.g = 0.5;
				ptColor.b = 0.0;
			}
			else if ( count > ( maxIter / 10) ) 
			{
				ptColor.r = 1.0;
				ptColor.g = 0.0;
				ptColor.b = 0.0;
			}
			else if ( count > ( maxIter / 20) ) 
			{
				ptColor.r = 0.0;
				ptColor.g = 0.0;
				ptColor.b = 0.5;
			}
			else if ( count > ( maxIter / 40) ) 
			{
				ptColor.r = 1.0;
				ptColor.g = 1.0;
				ptColor.b = 0.0;
			}
			else if ( count > ( maxIter / 100) ) 
			{
				ptColor.r = 0.0;
				ptColor.g = 0.3;
				ptColor.b = 0.0;
			}
			else 
			{
				ptColor.r = 0.0;
				ptColor.g = ptColor.b = 1.0;
			}
			z.color = ptColor;
			points.push_back(z);
		}
	}

	return points;
}

ComplexPoint Mandelbrot::ComplexSquare( ComplexPoint z )
{
	ComplexPoint square;

	square.x = z.x * z.x - z.y * z.y;
	square.y = 2 * z.x * z.y;

	return square;
}

int Mandelbrot::MandelbrotSqTransf( ComplexPoint z0, int maxIter ) 
{
	ComplexPoint z = z0;
	int count = 0;

	while ( ( z.x * z.x + z.y * z.y <= 4.0 ) && ( count < maxIter ) )
	{
		z = ComplexSquare( z );
		z.x += z0.x;
		z.y += z0.y;
		count++;
	}
	return count;
}


float Mandelbrot::GetComplexXMin()
{
	return this->ComplexXMin;
}

float Mandelbrot::GetComplexXMax()
{
	return this->ComplexXMax;
}

float Mandelbrot::GetComplexYMin()
{
	return this->ComplexYMin;
}

float Mandelbrot::GetComplexYMax()
{
	return this->ComplexYMax;
}


void Mandelbrot::SetComplexXMin( float xmin )
{
	this->ComplexXMin = xmin;
}

void Mandelbrot::SetComplexXMax( float xmax )
{
	this->ComplexXMax = xmax;
}

void Mandelbrot::SetComplexYMin( float ymin )
{
	this->ComplexYMin = ymin;
}

void Mandelbrot::SetComplexYMax( float ymax )
{
	this->ComplexYMax = ymax;
}

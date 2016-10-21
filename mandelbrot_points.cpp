#include "global.h"


Mandelbrot::Mandelbrot()
{
	// set xmin, ymin, xmax, ymax to default values
	this->ComplexXMin = -2;
	this->ComplexXMax = 1;
	this->ComplexYMin = -1.5;
	this->ComplexYMax = 1.5;
}

vector< ComplexPoint > Mandelbrot::GetPoints( int nx, int ny,  int maxIter ) 
{
	vector< ComplexPoint > points, gpupoints;

	ComplexPoint z, zIncr;
	int count = 0;

	// determine width and height of orthographic view
	ComplexWidth = ComplexXMax - ComplexXMin;
	ComplexHeight = ComplexYMax - ComplexYMin;

	// get y and y increments
	zIncr.x = ComplexWidth / double( nx );
	zIncr.y = ComplexHeight / double( ny );

	// for each row 
	for( z.x = ComplexXMin; z.x < ComplexXMax; z.x += zIncr.x )
	{
		// for each column
		for( z.y = ComplexYMin; z.y < ComplexYMax; z.y += zIncr.y ) 
		{
			// determine convergence/divergence
			count = MandelbrotSqTransf( z, maxIter );
			// set color scheme index
			if( count >= maxIter )
			{
				z.schemeIndex = 0;
			}
			else if ( count > ( maxIter / 8) ) 
			{
				z.schemeIndex = 2;
			}
			else if ( count > ( maxIter / 10) ) 
			{
				z.schemeIndex = 3;
			}
			else if ( count > ( maxIter / 20) ) 
			{
				z.schemeIndex = 4;
			}
			else if ( count > ( maxIter / 40) ) 
			{
				z.schemeIndex = 5;
			}
			else if ( count > ( maxIter / 100) ) 
			{
				z.schemeIndex = 6;
			}
			else if ( count > (maxIter / 200) )
			{
				z.schemeIndex = 7;
			}
			else if ( count > (maxIter / 400) )
			{
				z.schemeIndex = 8;
			}
			else if ( count > (maxIter / 600) )
			{
				z.schemeIndex = 9;
			}
			else if ( count > (maxIter / 800) )
			{
				z.schemeIndex = 1;
			}
			else 
			{
				z.schemeIndex = 10;
			}
			points.push_back(z);
		}
	}
	
	// return vector of points
	return points;
}

ComplexPoint Mandelbrot::ComplexSquare( ComplexPoint z )
{
	// performs z^2 on the passed point
	ComplexPoint square;

	square.x = z.x * z.x - z.y * z.y;
	square.y = 2 * z.x * z.y;

	return square;
}

int Mandelbrot::MandelbrotSqTransf( ComplexPoint z0, int maxIter ) 
{
	ComplexPoint z = z0;
	int count = 0;

	// iterates until we converge or diverge based on our maximum 
	// number of allowed interations
	while ( ( z.x * z.x + z.y * z.y <= 4.0 ) && ( count < maxIter ) )
	{
		z = ComplexSquare( z );
		z.x += z0.x;
		z.y += z0.y;
		count++;
	}
	return count;
}

// xmin, xmax, ymin, and ymax getters and setters
double Mandelbrot::GetComplexXMin()
{
	return this->ComplexXMin;
}

double Mandelbrot::GetComplexXMax()
{
	return this->ComplexXMax;
}

double Mandelbrot::GetComplexYMin()
{
	return this->ComplexYMin;
}

double Mandelbrot::GetComplexYMax()
{
	return this->ComplexYMax;
}

void Mandelbrot::SetComplexXMin( double xmin )
{
	this->ComplexXMin = xmin;
}


void Mandelbrot::SetComplexXMax( double xmax )
{
	this->ComplexXMax = xmax;
}

void Mandelbrot::SetComplexYMin( double ymin )
{
	this->ComplexYMin = ymin;
}

void Mandelbrot::SetComplexYMax( double ymax )
{
	this->ComplexYMax = ymax;
}

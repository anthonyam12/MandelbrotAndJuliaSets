#include "global.h"


Mandelbrot::Mandelbrot()
{
	this->ComplexXMin = -2;
	this->ComplexXMax = 1;
	this->ComplexYMin = -1.5;
	this->ComplexYMax = 1.5;
}

vector< ComplexPoint > Mandelbrot::GetPoints( int nx, int ny,  int maxIter, ColorScheme scheme ) 
{
	vector< ComplexPoint > points;

	ComplexPoint z, zIncr;
	int count = 0;

	ComplexWidth = ComplexXMax - ComplexXMin;
	ComplexHeight = ComplexYMax - ComplexYMin;

	zIncr.x = ComplexWidth / float( nx );
	zIncr.y = ComplexHeight / float( ny );

	for( z.x = ComplexXMin; z.x < ComplexXMax; z.x += zIncr.x )
	{
		for( z.y = ComplexYMin; z.y < ComplexYMax; z.y += zIncr.y ) 
		{
			count = MandelbrotSqTransf( z, maxIter );
			if( count >= maxIter )
			{
				z.color = scheme.black;
				z.schemeIndex = 0;
			}
			else if ( count > ( maxIter / 8) ) 
			{
				z.color = scheme.color1;
				z.schemeIndex = 1;
			}
			else if ( count > ( maxIter / 10) ) 
			{
				z.color = scheme.color2;
				z.schemeIndex = 2;
			}
			else if ( count > ( maxIter / 20) ) 
			{
				z.color = scheme.color3;
				z.schemeIndex = 3;
			}
			else if ( count > ( maxIter / 40) ) 
			{
				z.color = scheme.color4;
				z.schemeIndex = 4;
			}
			else if ( count > ( maxIter / 100) ) 
			{
				z.color = scheme.color5;
				z.schemeIndex = 5;
			}
			else if ( count > (maxIter / 200) )
			{
				z.color = scheme.color6;
				z.schemeIndex = 6;
			}
			else if ( count > (maxIter / 400) )
			{
				z.color = scheme.color7;
				z.schemeIndex = 7;
			}
			else if ( count > (maxIter / 600) )
			{
				z.color = scheme.color8;
				z.schemeIndex = 8;
			}
			else if ( count > (maxIter / 800) )
			{
				z.color = scheme.color9;
				z.schemeIndex = 9;
			}
			else 
			{
				z.color = scheme.color10;
				z.schemeIndex = 10;
			}
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

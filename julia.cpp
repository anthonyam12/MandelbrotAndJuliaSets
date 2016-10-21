#include "global.h"

Julia::Julia( ) 
{
	// initialize the x and y mins and maxs since the Julia
	// points lie in this range
	this->ComplexXMin = -2.0;
	this->ComplexXMax = 2.0;
	this->ComplexYMin = -2.00;
	this->ComplexYMax = 2.00;
}

vector< ComplexPoint > Julia::GetPoints( ComplexPoint seed, int nx, int ny, int maxIter )
{
	vector< ComplexPoint > points;
	ComplexPoint z,  zIncr;
	int count = 0;

	this->ComplexHeight = ComplexXMax - ComplexXMin;
	this->ComplexWidth = ComplexYMax - ComplexYMin;

	// get the increment in x and y based on number of steps
	zIncr.x = ComplexWidth / float( nx );
	zIncr.y = ComplexHeight / float( ny );

	// step through each row
	for( z.x = ComplexXMin; z.x < ComplexXMax; z.x += zIncr.x )
	{
		// step through each column
		for( z.y = ComplexYMin; z.y < ComplexYMax; z.y += zIncr.y ) 
		{
			// get count to determine if point converges or diverges
			count = JuliaSqTransf( z, seed, maxIter );
			// set color based on count returned
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

	// return vector of complex points
	return points;
}

// squares the complex number z
ComplexPoint Julia::ComplexSquare( ComplexPoint z )
{
	ComplexPoint square;

	square.x = z.x * z.x - z.y * z.y;
	square.y = 2 * z.x * z.y;

	return square;
}

// iterates and increments a counter. The count returned will determine the 
// color of the julia point
int Julia::JuliaSqTransf( ComplexPoint z0, ComplexPoint seed, int maxIter ) 
{
	ComplexPoint z = z0;
	int count = 0;

	while ( ( z.x * z.x + z.y * z.y <= 4.0 ) && ( count < maxIter ) )
	{
		z = ComplexSquare( z );
		z.x += seed.x;
		z.y += seed.y;
		count++;
	}
	return count;
}

// return xmin
double Julia::GetComplexXMin()
{
	return ComplexXMin;
}

// return xmax
double Julia::GetComplexXMax() 
{
	return ComplexXMax;
}

// return ymin
double Julia::GetComplexYMin()
{
	return ComplexYMin;
}

// return ymax
double Julia::GetComplexYMax() 
{
	return ComplexYMax;
}

// set xmin
void Julia::SetComplexXMin( double xmin )
{
	this->ComplexXMin = xmin;
}

// set xmax
void Julia::SetComplexXMax( double xmax )
{
	this->ComplexXMax = xmax;
}

// set ymin
void Julia::SetComplexYMin( double ymin )
{
	this->ComplexYMin = ymin;
}

// set ymax
void Julia::SetComplexYMax( double ymax )
{
	this->ComplexYMax = ymax;
}

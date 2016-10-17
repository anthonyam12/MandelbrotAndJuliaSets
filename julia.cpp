#include "global.h"

Julia::Julia( ) 
{
	this->ComplexXMin = -2.0;
	this->ComplexXMax = 2.0;
	this->ComplexYMin = -2.00;
	this->ComplexYMax = 2.00;
}

vector< ComplexPoint > Julia::GetPoints( ComplexPoint seed, int nx, int ny, 
										int maxIter, ColorScheme scheme )
{
	vector< ComplexPoint > points;
	ComplexPoint z,  zIncr;
	int count = 0;

	this->ComplexHeight = ComplexXMax - ComplexXMin;
	this->ComplexWidth = ComplexYMax - ComplexYMin;

	zIncr.x = ComplexWidth / float( nx );
	zIncr.y = ComplexHeight / float( ny );

	for( z.x = ComplexXMin; z.x < ComplexXMax; z.x += zIncr.x )
	{
		for( z.y = ComplexYMin; z.y < ComplexYMax; z.y += zIncr.y ) 
		{
			//TODO: should we use more subdivisions for more definition?
			count = JuliaSqTransf( z, seed, maxIter );
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

ComplexPoint Julia::ComplexSquare( ComplexPoint z )
{
	ComplexPoint square;

	square.x = z.x * z.x - z.y * z.y;
	square.y = 2 * z.x * z.y;

	return square;
}

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

double Julia::GetComplexXMin()
{
	return ComplexXMin;
}

double Julia::GetComplexXMax() 
{
	return ComplexXMax;
}

double Julia::GetComplexYMin()
{
	return ComplexYMin;
}

double Julia::GetComplexYMax() 
{
	return ComplexYMax;
}

void Julia::SetComplexXMin( double xmin )
{
	this->ComplexXMin = xmin;
}


void Julia::SetComplexXMax( double xmax )
{
	cout << "here" << xmax << endl;
	this->ComplexXMax = xmax;
}

void Julia::SetComplexYMin( double ymin )
{
	this->ComplexYMin = ymin;
}

void Julia::SetComplexYMax( double ymax )
{
	this->ComplexYMax = ymax;
}

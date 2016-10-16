#include "global.h"

void Julia::SetComplexGlobals( ComplexPoint seed ) 
{
	this->ComplexXMin = -2.0;
	this->ComplexXMax = 2.0;
	this->ComplexYMin = -2.00;
	this->ComplexYMax = 2.00;

	this->ComplexHeight = ComplexXMax - ComplexXMin;
	this->ComplexWidth = ComplexYMax - ComplexYMin;
}

vector< ComplexPoint > Julia::GetPoints( ComplexPoint seed, int nx, int ny,  int maxIter )
{
	SetComplexGlobals( seed );
	vector< ComplexPoint > points;
	Color ptColor;
	ComplexPoint z,  zIncr;
	int count = 0;

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
			else if ( count > (maxIter / 200) )
			{
				ptColor.r = 0.0;
				ptColor.g = 0.3;
				ptColor.b = 0.3;
			}
			else if ( count > (maxIter / 400) )
			{
				ptColor.r = 0.0;
				ptColor.g = 0.5;
				ptColor.b = 0.5;
			}
			else if ( count > (maxIter / 600) )
			{
				ptColor.r = 0.0;
				ptColor.g = 0.7;
				ptColor.b = 0.7;
			}
			else if ( count > (maxIter / 800) )
			{
				ptColor.r = 0.0;
				ptColor.g = 0.9;
				ptColor.b = 0.9;
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

float Julia::GetComplexXMin()
{
	return ComplexXMin;
}

float Julia::GetComplexXMax() 
{
	return ComplexXMax;
}

float Julia::GetComplexYMin()
{
	return ComplexYMin;
}

float Julia::GetComplexYMax() 
{
	return ComplexYMax;
}

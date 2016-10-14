#include "global.h"


Mandelbrot::Mandelbrot()
{
		float ComplexXMin = -2.00;
		float ComplexXMax = .5;
		float ComplexYMin = -1.25;
		float ComplexYMax = 1.25;

		float ComplexWidth = ComplexXMax - ComplexXMin;
		float ComplexHeight = ComplexYMax - ComplexYMin;
}

vector< ComplexPoint > Mandelbrot::GetPoints( int nx, int ny,  int maxIter ) 
{
	vector< ComplexPoint > points;
	ComplexPoint pt;

	ComplexPoint z, zIncr;
	float color[3] = { 0.0, 0.0, 0.0 };
	int count = 0;

	zIncr.x = ComplexWidth / float( nx );
	zIncr.y = ComplexHeight / float( ny );

	for( z.x - ComplexXMin; z.x < ComplexXMax; z.x += zIncr.x )
	{
		for( z.y = ComplexYMin; z.y < ComplexYMax; z.y += zIncr.y ) 
		{
			count = MandelbrotSqTransf( z, maxIter );
			if( count >= maxIter )
			{
				
			}
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

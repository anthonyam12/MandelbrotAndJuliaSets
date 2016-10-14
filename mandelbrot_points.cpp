#include "points.h"


vector< ComplexPoint > Mandelbrot::GetPoints( int minx, int maxx, int miny, int maxy, int maxIters ) 
{
	vector< ComplexPoint > points;
	ComplexPoint pt;

	pt.x = 1;
	pt.y = 4;
	points.push_back(pt);

	return points;
}

ComplexPoint Mandelbrot::ComplexSquare( ComplexPoint z )
{
	ComplexPoint square;

	square.x = z.x * z.x - z.y * z.y;
	square.y = 2 * z.x * z.y;

	return square;
}

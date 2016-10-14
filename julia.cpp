#include "global.h"

vector< ComplexPoint > Julia::GetPoints( int nx, int ny,  int maxIters )
{
	vector< ComplexPoint > points;
	ComplexPoint pt;

	pt.x = 1;
	pt.y = 2;

	points.push_back(pt);

	return points;
}


ComplexPoint Julia::ComplexSquare( ComplexPoint z )
{
	ComplexPoint square;

	square.x = z.x * z.x - z.y * z.y;
	square.y = 2 * z.x * z.y;

	return square;
}

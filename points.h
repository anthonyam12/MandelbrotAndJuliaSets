#include <vector>
#include <iostream>

using namespace std;

struct ComplexPoint 
{
	float x;
	float y;
};

class Julia 
{
	public:
		vector< ComplexPoint > GetPoints( int minx, int maxx, int miny, int maxy, int maxIters );
	private:
		ComplexPoint ComplexSquare( ComplexPoint z );
};

class Mandelbrot
{
	public:
		vector< ComplexPoint > GetPoints( int minx, int maxx, int miny, int maxy, int maxIters );
	private:
		ComplexPoint ComplexSquare( ComplexPoint z );
};

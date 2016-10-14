#include <vector>
#include <iostream>

using namespace std;

struct Color
{
	float r;
	float g;
	float b;
};

struct ComplexPoint 
{
	float x;
	float y;
	Color color;
};

class Julia 
{
	public:
		vector< ComplexPoint > GetPoints( int nx, int ny, int maxIters );
	private:
		ComplexPoint ComplexSquare( ComplexPoint z );
};

class Mandelbrot
{
	public:
		Mandelbrot();
		vector< ComplexPoint > GetPoints( int nx, int ny, int maxIters );
		float GetComplexXMin();
		float GetComplexXMax();
		float GetComplexYMin();
		float GetComplexYMax();
	private:
		ComplexPoint ComplexSquare( ComplexPoint z );
		int MandelbrotSqTransf( ComplexPoint z, int maxIter );
		float ComplexXMin;
		float ComplexXMax;
		float ComplexYMin;
		float ComplexYMax;
		float ComplexWidth;
		float ComplexHeight;
};

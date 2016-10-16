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
		vector< ComplexPoint > GetPoints( ComplexPoint seed, int nx, int ny, int maxIters );
		float GetComplexXMin();
		float GetComplexXMax();
		float GetComplexYMin();
		float GetComplexYMax();
		// may not need these
		void SetComplexXMin( float xmin );
		void SetComplexXMax( float xmax );
		void SetComplexYMin( float ymin );
		void SetComplexYMax( float ymax );
	private:
		ComplexPoint ComplexSquare( ComplexPoint z );
		int JuliaSqTransf( ComplexPoint z, ComplexPoint seed, int maxIter );
		void SetComplexGlobals( ComplexPoint seed );

		float ComplexXMin;
		float ComplexXMax;
		float ComplexYMin;
		float ComplexYMax;
		float ComplexWidth;
		float ComplexHeight;
};

class Julia_cu
{
	public:
		vector< ComplexPoint > GetPoints( ComplexPoint seed, int nx, int ny, int maxIters );
	private:
		ComplexPoint ComplexSquare( ComplexPoint z );
};

class Mandelbrot
{
	public:
		Mandelbrot();
		vector< ComplexPoint > GetPoints( int nx, int ny, int maxIters, Color colorScheme[7] );
		
		float GetComplexXMin();
		float GetComplexXMax();
		float GetComplexYMin();
		float GetComplexYMax();

		// may not need these
		void SetComplexXMin( float xmin );
		void SetComplexXMax( float xmax );
		void SetComplexYMin( float ymin );
		void SetComplexYMax( float ymax );
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

class Mandelbrot_cu
{	
	public:
		Mandelbrot_cu();
		//void CalcPoint( float *x, float *y, float *r, float *g, float *b, float w, float h );
		vector< ComplexPoint > GetPoints( int nx, int ny, int maxIters, Color colorScheme[7] );
		
		float GetComplexXMin();
		float GetComplexXMax();
		float GetComplexYMin();
		float GetComplexYMax();

		// may not need these
		void SetComplexXMin( float xmin );
		void SetComplexXMax( float xmax );
		void SetComplexYMin( float ymin );
		void SetComplexYMax( float ymax );
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


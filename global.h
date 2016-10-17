#include <vector>
#include <iostream>

using namespace std;

class Color
{
	public:
		Color();
		Color( float r, float g, float b );
		bool equals( Color color );
		float r;
		float g;
		float b;
};

struct ColorScheme 
{
	Color black;	// in the set
	Color color1;
	Color color2;
	Color color3;
	Color color4;
	Color color5;
	Color color6;
	Color color7;
	Color color8;
	Color color9;
	Color color10;
};

struct ComplexPoint 
{
	double x;
	double y;
	Color color;
	int schemeIndex;
};

class Julia 
{
	public:
		Julia();
		vector< ComplexPoint > GetPoints( ComplexPoint seed, int nx, int ny, int maxIters, ColorScheme scheme );
		double GetComplexXMin();
		double GetComplexXMax();
		double GetComplexYMin();
		double GetComplexYMax();

		void SetComplexXMin( double xmin );
		void SetComplexXMax( double xmax );
		void SetComplexYMin( double ymin );
		void SetComplexYMax( double ymax );
	private:
		ComplexPoint ComplexSquare( ComplexPoint z );
		int JuliaSqTransf( ComplexPoint z, ComplexPoint seed, int maxIter );

		double ComplexXMin;
		double ComplexXMax;
		double ComplexYMin;
		double ComplexYMax;
		double ComplexWidth;
		double ComplexHeight;
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
		vector< ComplexPoint > GetPoints( int nx, int ny, int maxIters, ColorScheme scheme );
		
		double GetComplexXMin();
		double GetComplexXMax();
		double GetComplexYMin();
		double GetComplexYMax();

		void SetComplexXMin( double xmin );
		void SetComplexXMax( double xmax );
		void SetComplexYMin( double ymin );
		void SetComplexYMax( double ymax );
	private:
		ComplexPoint ComplexSquare( ComplexPoint z );
		int MandelbrotSqTransf( ComplexPoint z, int maxIter );
		double ComplexXMin;
		double ComplexXMax;
		double ComplexYMin;
		double ComplexYMax;
		double ComplexWidth;
		double ComplexHeight;
};

class Mandelbrot_cu
{	
	public:
		Mandelbrot_cu();
		//void CalcPoint( float *x, float *y, float *r, float *g, float *b, float w, float h );
		vector< ComplexPoint > GetPoints( int nx, int ny, int maxIters, Color colorScheme[7] );
		
		double GetComplexXMin();
		double GetComplexXMax();
		double GetComplexYMin();
		double GetComplexYMax();

		void SetComplexXMin( double xmin );
		void SetComplexXMax( double xmax );
		void SetComplexYMin( double ymin );
		void SetComplexYMax( double ymax );
	private:
		ComplexPoint ComplexSquare( ComplexPoint z );
		int MandelbrotSqTransf( ComplexPoint z, int maxIter );
		double ComplexXMin;
		double ComplexXMax;
		double ComplexYMin;
		double ComplexYMax;
		double ComplexWidth;
		double ComplexHeight;
};


#include <vector>
#include <iostream>
#include <stdexcept>

using namespace std;

// Color class to store color values and supply an equal function
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

// stores the 11 colors of a color scheme, has getter and setter
// functionality, and supplies and equal method
class ColorScheme 
{
	public:
		ColorScheme();
		ColorScheme( Color black, Color color1, Color color2, Color color3,
					 Color color4, Color color5, Color color6, Color color7,
					 Color color8, Color color9, Color color10);
		void SetColor( int colorIndex, Color newColor );
		Color GetColor( int colorIndex );
		bool Equals( ColorScheme scheme );
	private:
		Color black;
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

// x and y of the complex point x + iy
// schemeIndex - index into the color scheme (determines color of the point)
struct ComplexPoint 
{
	double x;
	double y;
	int schemeIndex;
};

// CPU julia class to get the Julia points and set color scheme idnex of the points
class Julia 
{
	public:
		Julia();
		vector< ComplexPoint > GetPoints( ComplexPoint seed, int nx, int ny, int maxIters );
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

// GPU Julia class to get the colors of the Julia set points
class Julia_cu
{
	public:
		Julia_cu();
		vector< ComplexPoint > GetPoints( ComplexPoint seed, int nx, int ny, int maxIters );
		double GetComplexXMin();
		double GetComplexXMax();
		double GetComplexYMin();
		double GetComplexYMax();

		void SetComplexXMin( double xmin );
		void SetComplexXMax( double xmax );
		void SetComplexYMin( double ymin );
		void SetComplexYMax( double ymax );
	private:
		double ComplexXMin;
		double ComplexXMax;
		double ComplexYMin;
		double ComplexYMax;
		double ComplexWidth;
		double ComplexHeight;
};

// CPU Mandelbrot to get the colors of the points
class Mandelbrot
{
	public:
		Mandelbrot();
		vector< ComplexPoint > GetPoints( int nx, int ny, int maxIters );
		
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

// GPU Mandelbrot to set the colors of the mandelbrot points
class Mandelbrot_cu
{	
	public:
		Mandelbrot_cu();
		vector< ComplexPoint > GetPoints( int nx, int ny, int maxIters );
		
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


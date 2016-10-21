#include "global.h"

Julia_cu::Julia_cu()
{
	// sets the default xmin, xmax, ymin, and ymax sets the julia set 
	// points are in this range
	this->ComplexXMin = -2.0;
	this->ComplexXMax = 2.0;
	this->ComplexYMin = -2.0;
	this->ComplexYMax = 2.0;
}

// Ran on the GPU, sets the scheme[] value for each points. This value is used
// as an index into the ColorScheme class to determine the color of a point
__global__ void CalcPoint( float *x, float *y, int *scheme, int nx, 
						   int ny, int maxIter, double seedx, double seedy )
{
	// get the index
	int index = threadIdx.x + blockIdx.x * blockDim.x;

	// determine if we need to run this index (in our range)
	if ( index < nx )
	{
		// start and end position in the y array
		int start = index*ny;
		int end = start + ny;

		// iterate the columns for this row
		for ( int i = start; i < end; i++ )
		{
			// get this x + yi point
			double zx = x[index];
			double zy = y[i];

			int count = 0;
			
			// determine convergence/divergence
			while( ( zx*zx + zy*zy <= 4.0 ) && ( count < maxIter ) )
			{
				// complex square
				double new_zx = zx*zx - zy*zy;
				double new_zy = 2 * zx * zy;
				zx = new_zx;
				zy = new_zy;

				// add z0
				zx += seedx;
				zy += seedy;

				// incr count
				count++;
			}
		
			// set color
			if( count >= maxIter )
			{
				scheme[i] = 0;
			}
			else if ( count > ( maxIter / 8) ) 
			{
				scheme[i] = 2;
			}
			else if ( count > ( maxIter / 10) ) 
			{
				scheme[i] = 3;
			}
			else if ( count > ( maxIter / 20) ) 
			{
				scheme[i] = 4;
			}
			else if ( count > ( maxIter / 40) ) 
			{
				scheme[i] = 5;
			}
			else if ( count > ( maxIter / 100) ) 
			{
				scheme[i] = 6;
			}
			else if ( count > (maxIter / 200) )
			{
				scheme[i] = 7;
			}
			else if ( count > (maxIter / 400) )
			{
				scheme[i] = 8;
			}
			else if ( count > (maxIter / 600) )
			{
				scheme[i] = 9;
			}
			else if ( count > (maxIter / 800) )
			{
				scheme[i] = 1;
			}
			else 
			{
				scheme[i] = 10;
			}
		}
	}
}


vector< ComplexPoint > Julia_cu::GetPoints( ComplexPoint seed, int nx, int ny,  int maxIter )
{
	vector< ComplexPoint > points;
	ComplexPoint zIncr, z;
	
	// get size of the arrays
	int size_nx = nx * sizeof( float );
	int size_nynx = ny * nx * sizeof( float );
	int size_sch = ny * nx * sizeof( int );

	// create x,y point arrays and a scheme array to store indexes into color scheme class
	float *x = ( float* )malloc(size_nx);
	float *y = ( float* )malloc(size_nynx);
	int *scheme = ( int* )malloc(size_sch);

	// determine the width and height of the screen
	ComplexWidth = ComplexXMax - ComplexXMin;
	ComplexHeight = ComplexYMax - ComplexYMin;
	
	// determine the increments in x and y
	zIncr.x = ComplexWidth / float( nx );
	zIncr.y = ComplexHeight / float( ny );

	// fill the x and y arrays with each x and y point we are working with
	for( int i = 0; i < nx; i++ )
	{
		x[i] = ComplexXMin + ( zIncr.x * i );
		int multiplier = 0;
		for( int j = i*ny; j < (i+1)*ny; j++ )
		{
			y[j] = ComplexYMin + ( zIncr.y * multiplier );
			scheme[i] = 0;
			multiplier++;
		}
	}

	// create the device x,y, and scheme arrays
	float *d_x, *d_y;
	int *d_scheme;

	// allocate the device memory
	cudaMalloc( ( void** )&d_x, size_nx );
	cudaMalloc( ( void** )&d_y, size_nynx );
	cudaMalloc( ( void** )&d_scheme, size_sch );

	// copy the arrays to the device
	cudaMemcpy( d_x, x, size_nx, cudaMemcpyHostToDevice );
	cudaMemcpy( d_y, y, size_nynx, cudaMemcpyHostToDevice );
	cudaMemcpy( d_scheme, scheme, size_nynx, cudaMemcpyHostToDevice );

	// set number of blocks and threads on GPU
	int nThreads = 64;
	int nBlocks = ( nx + nThreads - 1 ) / nThreads;

	// calculate points on the GPU
	CalcPoint<<< nBlocks, nThreads >>>( d_x, d_y, d_scheme, nx, ny, maxIter, seed.x, seed.y );
	
	// copy memory back to host
	cudaMemcpy( x, d_x, size_nx, cudaMemcpyDeviceToHost );
	cudaMemcpy( y, d_y, size_nynx, cudaMemcpyDeviceToHost );
	cudaMemcpy( scheme, d_scheme, size_nynx, cudaMemcpyDeviceToHost );

	// fill the point vector with x,y points and the scheme indexes
	for( int i = 0; i < nx; i++ )
	{
		z.x = x[i];
		for ( int j = i*ny; j < (i+1)*ny; j++ )
		{
			z.y = y[j];
			z.schemeIndex = scheme[j];
			points.push_back( z );
		}
	}

	// free the memory
	free(x); free(y); free(scheme);
	cudaFree(d_x); cudaFree(d_y); cudaFree(d_scheme);
	return points;
}

// getters and setters for the xmin, xmax, ymin, and ymax
double Julia_cu::GetComplexXMin()
{
	return ComplexXMin;
}

double Julia_cu::GetComplexXMax() 
{
	return ComplexXMax;
}

double Julia_cu::GetComplexYMin()
{
	return ComplexYMin;
}

double Julia_cu::GetComplexYMax() 
{
	return ComplexYMax;
}

void Julia_cu::SetComplexXMin( double xmin )
{
	this->ComplexXMin = xmin;
}

void Julia_cu::SetComplexXMax( double xmax )
{
	this->ComplexXMax = xmax;
}

void Julia_cu::SetComplexYMin( double ymin )
{
	this->ComplexYMin = ymin;
}

void Julia_cu::SetComplexYMax( double ymax )
{
	this->ComplexYMax = ymax;
}

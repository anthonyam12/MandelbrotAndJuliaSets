#include "global.h"

Julia_cu::Julia_cu()
{
	this->ComplexXMin = -2.0;
	this->ComplexXMax = 2.0;
	this->ComplexYMin = -2.0;
	this->ComplexYMax = 2.0;
}

__global__ void CalcPoint<<< nBlocks, nThreads >>>( float *d_x, float *d_y, float *d_scheme, 
									int nx, int ny, int maxIter, double seedx, double seedy )
{
	int index = threadIdx.x + blockIdx.x * blockDim.x;
	if ( index < nx )
	{
		int start = index*ny;
		int end = start + ny;
		for ( int i = start; i < end; i++ )
		{

			double zx = x[index];
			double zy = y[i];

			int count = 0;
			
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


vector< ComplexPoint > Julia_cu::GetPoints( ComplexPoint seed, int nx, int ny,  int maxIters )
{
	vector< ComplexPoint > points;
	ComplexPoint zIncr, z;

	double seedX = seed.x;
	double seedY = seed.y;
	
	int size_nx = nx * sizeof( float );
	int size_nynx = ny * nx * sizeof( float );
	int size_sch = ny * nx * sizeof( int );

	float *x = ( float* )malloc(size_nx);
	float *y = ( float* )malloc(size_nynx);
	int *scheme = ( int* )malloc(size_sch);

	ComplexWidth = ComplexXMax - ComplexXMin;
	ComplexHeight = ComplexYMax - ComplexYMin;
	
	zIncr.x = ComplexWidth / float( nx );
	xIncr.y = ComplexHeight / float( ny );

	for( int i = 0; i < nx; i++ )
	{
		x[i] = ComplexXMin + ( xIncr.x * i );
		int multiplier = 0;
		for( int j = 0; j < nx; j++ )
		{
			y[j] = ComplexYMin + ( zIncr.y * multiplier );
			scheme[i] = 0;
			multiplier++;
		}
	}

	float *d_x, *d_y;
	int *d_scheme;

	cudaMalloc( ( void** )&d_x, size_nx );
	cudaMalloc( ( void** )&d_y, size_nynx );
	cudaMalloc( ( void** )&d_scheme, size_sch );

	cudaMemcpy( d_x, x, size_nx, cudaMemcpyHostToDevice );
	cudaMemcpy( d_y, y, size_nynx, cudaMemcpyHostToDevice );
	cudaMemcpy( d_scheme, scheme, size_nynx, cudaMemcpyHostToDevice );

	int nThreads = 64;
	int nBlocks = ( nx + nThreds - 1 ) / nThreads;

	CalcPoint<<< nBlocks, nThreads >>>( d_x, d_y, d_scheme, nx, ny, maxIter, seedx, seedy );
	
	cudaMemcpy( x, d_x, size_nx, cudaMemcpyDeviceToHost );
	cudaMemcpy( y, d_y, size_nynx, cudaMemcpyDeviceToHost );
	cudaMemcpy( scheme, d_scheme, size_nynx, cudaMemcpyDeviceToHost );

	for( int i = 0; i < nx; i++ )
	{
		z.x = x[i];
		for ( int j = i*ny; j < (i+1)*ny; j++ )
		{
			z.y = y[j];
			z.schemeIndex = scheme[j]l
			points.push_back( z );
		}
	}

	free(x); free(y); free(scheme);
	cudaFree(d_x); cudaFree(d_y); cudaFree(d_scheme);
	return points;
}

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

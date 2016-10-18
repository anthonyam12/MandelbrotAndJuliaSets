#include "global.h"

Mandelbrot_cu::Mandelbrot_cu()
{
		this->ComplexXMin = -2.00;
		this->ComplexXMax = 1;
		this->ComplexYMin = -1.5;
		this->ComplexYMax = 1.5;
}

__global__ void CalcPoint( float *x, float *y, int *scheme, int nx, int ny, int maxIter ) 
{
	int index = threadIdx.x + blockIdx.x * blockDim.x;
	if ( index < nx ) 
	{
		for( int i = 0; i < ny; i++ )
		{
			float zx0 = x[index];
			float zy0 = y[index + i];

			float zx = x[index];
			float zy = y[index + i];

			int count = 0;
			while( ( zx*zx + zy*zy <= 4.0 ) && ( count < maxIter ) )
			{
				// complex square
				float new_zx = zx*zx - zy*zy;
				float new_zy = 2 * zx * zy;
				zx = new_zx;
				zy = new_zy;

				// add z0
				zx += zx0;
				zy += zy0;

				// incr count
				count++;
			}

			// set color
			if( count >= maxIter )
			{
				scheme[index + i] = 0;
			}
			else if ( count > ( maxIter / 8) ) 
			{
				scheme[index + i] = 2;
			}
			else if ( count > ( maxIter / 10) ) 
			{
				scheme[index + i] = 3;
			}
			else if ( count > ( maxIter / 20) ) 
			{
				scheme[index + i] = 4;
			}
			else if ( count > ( maxIter / 40) ) 
			{
				scheme[index + i] = 5;
			}
			else if ( count > ( maxIter / 100) ) 
			{
				scheme[index + i] = 6;
			}
			else if ( count > (maxIter / 200) )
			{
				scheme[index + i] = 7;
			}
			else if ( count > (maxIter / 400) )
			{
				scheme[index + i] = 8;
			}
			else if ( count > (maxIter / 600) )
			{
				scheme[index + i] = 9;
			}
			else if ( count > (maxIter / 800) )
			{
				scheme[index + i] = 1;
			}
			else 
			{
				scheme[index + i] = 10;
			}
		}
	}
}

vector< ComplexPoint > Mandelbrot_cu::GetPoints( int nx, int ny, int maxIter ) 
{
	vector< ComplexPoint > points;

	int size_nx = nx * sizeof( float );
	int size_nynx = ( nx*ny ) * sizeof( float );
	int size_sch = ( nx*ny ) * sizeof( int );	

	float *x = ( float * )malloc(size_nx);
	float *y = ( float * )malloc(size_nynx);
	int *scheme = ( int *)malloc(size_sch);
	
	// fill arrays with points before passing
	ComplexPoint z, zIncr;
	
	ComplexWidth = ComplexXMax - ComplexXMin;
	ComplexHeight = ComplexYMax - ComplexYMin;

	zIncr.x = ComplexWidth / double( nx );
	zIncr.y = ComplexHeight / double( ny );

	for( int i = 0; i < nx; i++ )
	{
		// get and set complex x value
		x[i] = ComplexXMin + ( zIncr.x * i );
		for( int j = 0; j < ny; j++ )
		{
			// get and set complex y value (and default scheme)
			y[j+i] = ComplexYMin + ( zIncr.y * j);
			scheme[j] = 0;
		}
	}

	// Do host side CUDA prep and run kernel on CUDA device
	float *d_x, *d_y;
	int *d_scheme;
	cudaMalloc( ( void** )&d_x, size_nx );
	cudaMalloc( ( void** )&d_y, size_nynx );
	cudaMalloc( ( void** )&d_scheme, size_sch );

	cudaMemcpy( d_x, x, size_nx, cudaMemcpyHostToDevice );
	cudaMemcpy( d_y, y, size_nynx, cudaMemcpyHostToDevice );
	cudaMemcpy( d_scheme, scheme, size_sch,  cudaMemcpyHostToDevice );

	int nThreads = 64;
	int nBlocks = ( nx + nThreads - 1 ) / nThreads;

	CalcPoint<<< nBlocks, nThreads >>>( d_x, d_y, d_scheme, nx, ny, maxIter );

	cudaMemcpy( x, d_x, size_nx, cudaMemcpyDeviceToHost );
	cudaMemcpy( y, d_y, size_nynx, cudaMemcpyDeviceToHost );
	cudaMemcpy( scheme, d_scheme, size_sch, cudaMemcpyDeviceToHost );


	for( int i = 0; i < nx; i++ )
	{
		z.x = x[i];
		for( int j = 0; j < ny; j++ )
		{
			z.y = y[i + j];
			z.schemeIndex = scheme[i + j];
			points.push_back(z);
		}
	}
	
	free(x); free(y); free(scheme);
	cudaFree(d_x); cudaFree(d_y); cudaFree(d_scheme);
	return points;
}

double Mandelbrot_cu::GetComplexXMin()
{
	return this->ComplexXMin;
}

double Mandelbrot_cu::GetComplexXMax()
{
	return this->ComplexXMax;
}

double Mandelbrot_cu::GetComplexYMin()
{
	return this->ComplexYMin;
}

double Mandelbrot_cu::GetComplexYMax()
{
	return this->ComplexYMax;
}


void Mandelbrot_cu::SetComplexXMin( double xmin )
{
	this->ComplexXMin = xmin;
}

void Mandelbrot_cu::SetComplexXMax( double xmax )
{
	this->ComplexXMax = xmax;
}

void Mandelbrot_cu::SetComplexYMin( double ymin )
{
	this->ComplexYMin = ymin;
}

void Mandelbrot_cu::SetComplexYMax( double ymax )
{
	this->ComplexYMax = ymax;
}

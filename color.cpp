#include "global.h"

Color::Color()
{
	// set the colors to black if no values passed
	this->r = 0.0;
	this->g = 0.0;
	this->b = 0.0;
}

Color::Color( float r, float g, float b )
{
	// create a new color with the values supplied
	this->r = r;
	this->g = g;
	this->b = b;
}

bool Color::equals( Color color )
{
	// determines if this color is equal to the passed color
	return this->r == color.r &&
		   this->g == color.g &&
		   this->b == color.b;
}

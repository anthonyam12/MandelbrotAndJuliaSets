#include "global.h"


ColorScheme::ColorScheme()
{
}

ColorScheme::ColorScheme( Color black, Color color1, Color color2, Color color3,
					 	  Color color4, Color color5, Color color6, Color color7,
					 	  Color color8, Color color9, Color color10)
{
	this->black = black;
	this->color1 = color1;
	this->color2 = color2;
	this->color3 = color3;
	this->color4 = color4;
	this->color5 = color5;
	this->color6 = color6;
	this->color7 = color7;
	this->color8 = color8;
	this->color9 = color9;
	this->color10 = color10;
}

void ColorScheme::SetColor( int colorIndex, Color newColor )
{
	switch( colorIndex )
	{
		case 0:
			black = newColor;
			break;
		case 1:
			color1 = newColor;
			break;
		case 2:
			color2 = newColor;
			break;
		case 3:
			color3 = newColor;
			break;
		case 4:
			color4 = newColor;
			break;
		case 5:
			color5 = newColor;
			break;
		case 6:
			color6 = newColor;
			break;
		case 7:
			color7 = newColor;
			break;
		case 8:
			color8 = newColor;
			break;
		case 9:
			color9 = newColor;
			break;
		case 10:
			color10 = newColor;
			break;
		default:
			throw invalid_argument( "In ColorScheme#GetColor: ColorIndex must be between 0 and 10" );
	}
}

Color ColorScheme::GetColor( int colorIndex )
{
	switch( colorIndex )
	{
		case 0:
			return black;
		case 1:
			return color1;
		case 2:
			return color2;
		case 3:
			return color3;
		case 4:
			return color4;
		case 5:
			return color5;
		case 6:
			return color6;
		case 7:
			return color7;
		case 8:
			return color8;
		case 9:
			return color9;
		case 10:
			return color10;
		default:
			throw invalid_argument( "In ColorScheme#GetColor: ColorIndex must be between 0 and 10" );
	}
}

bool ColorScheme::Equals( ColorScheme scheme )
{
	return scheme.color1.equals( color1 ) &&
		   scheme.color2.equals( color2 ) &&
		   scheme.color3.equals( color3 ) &&
		   scheme.color4.equals( color4 ) &&
		   scheme.color5.equals( color5 ) &&
		   scheme.color6.equals( color6 ) &&
		   scheme.color7.equals( color7 ) &&
		   scheme.color8.equals( color8 ) &&
		   scheme.color9.equals( color9 ) &&
		   scheme.color10.equals( color10 );
}

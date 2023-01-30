#include "DecodedImage.h"

DecodedImage::DecodedImage(int _width, int _height)
{
	width = _width;
	height = _height;
	pixels = new float[(width * height) * 3];
}

DecodedImage::~DecodedImage()
{
	delete[] pixels;
}

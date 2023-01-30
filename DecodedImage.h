#pragma once

class DecodedImage
{
public:
	float* pixels = nullptr;
	int width;
	int height;

	DecodedImage(int _width, int _height);

private:
	~DecodedImage();
};
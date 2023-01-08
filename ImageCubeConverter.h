#pragma once
#include "CubeConverter.h"

class ImageCubeConverter : public CubeConverter
{
public:
	ImageCubeConverter(std::string path, ConvertOptions options);

private:
	int faceIds[6];
};


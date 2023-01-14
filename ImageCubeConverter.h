#pragma once
#include "CubeConverter.h"
#include "il.h"

class ImageCubeConverter : public CubeConverter
{
public:
	ImageCubeConverter(std::string path, ConvertOptions* options);

protected:
	

private:
	ILuint faceIds[6];
};


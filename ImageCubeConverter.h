#pragma once
#include "CubeConverter.h"
#include "il.h"

class ImageCubeConverter : public CubeConverter
{
public:
	ImageCubeConverter(std::string path, ConvertOptions* options);

protected:
	void GetCubeFaceSize(int cubeFace, int* width, int* height);
	void SetTargetPixel(int x, int y, void* colour);

private:
	ILuint faceIds[6];
};


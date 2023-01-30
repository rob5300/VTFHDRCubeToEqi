#pragma once
#include "CubeConverter.h"
#include "il.h"
#include "DecodedImage.h"

class ImageCubeConverter : public CubeConverter
{
public:
	ImageCubeConverter(std::string path, ConvertOptions* options);
	bool Convert(std::string* faces) override;

protected:
	
	void GetCubeFaceSize(int cubeFace, int* width, int* height);
	float* GetSourcePixel(int x, int y, int cubeFace);

private:
	ILuint faceIds[6];
	DecodedImage* decodedFaces[6];
	~ImageCubeConverter();
};


#pragma once
#include "CubeConverter.h"

class VTFCubeConverter : public CubeConverter
{
public:
	VTFCubeConverter(std::string path, ConvertOptions* options);
	bool Convert(std::string* faces) override;

protected:
	unsigned char* GetSourcePixel(float x, float y, int cubeFace) override;
	void SetTargetPixel(int x, int y, void* colour) override;

private:
	VTFLib::CVTFFile faceVTFs[6];
};


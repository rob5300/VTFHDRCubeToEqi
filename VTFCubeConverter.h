#pragma once
#include "CubeConverter.h"

class VTFCubeConverter : public CubeConverter
{
public:
	VTFCubeConverter(std::string path, ConvertOptions* options);
	bool Convert(std::string* faces) override;

protected:
	unsigned char* GetSourcePixel(int x, int y, int cubeFace) override;
	void SetTargetPixel(int x, int y, void* colour) override;
	void GetCubeFaceSize(int cubeFace, int* width, int* height);

private:
	VTFLib::CVTFFile faceVTFs[6];
};


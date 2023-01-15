#include "ImageCubeConverter.h"

ImageCubeConverter::ImageCubeConverter(std::string path, ConvertOptions* options) : CubeConverter(path, options)
{
}

void ImageCubeConverter::GetCubeFaceSize(int cubeFace, int* width, int* height)
{}

void ImageCubeConverter::SetTargetPixel(int x, int y, void* colour)
{}

float* ImageCubeConverter::GetSourcePixel(int x, int y, int cubeFace)
{
    return nullptr;
}



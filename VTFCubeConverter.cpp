#define _USE_MATH_DEFINES
#include <math.h>
#include "VTFCubeConverter.h"

using namespace std;

VTFCubeConverter::VTFCubeConverter(std::string path, ConvertOptions* options) : CubeConverter(path, options)
{
}

bool VTFCubeConverter::Convert(std::string* faces)
{
    //Verify faces
    for (int i = 0; i < 6; i++)
    {
        if (faces[i].length() == 0)
        {
            printf("Failed to find all 6 faces of cubemap.");
            return false;
        }
    }

    printf("Found all 6 cubemap faces\n");

    int maxSize = 0;

    for (int i = 0; i < 6; i++)
    {
        auto success = faceVTFs[i].Load(faces[i].c_str());
        if (!success)
        {
            printf("Error: Unable to load image '%s'\n", faces[i]);
            return false;
        }
        else
        {
            maxSize = max(maxSize, (int)faceVTFs[i].GetWidth());
            maxSize = max(maxSize, (int)faceVTFs[i].GetHeight());
        }
    }

    DoConvertion(maxSize);
}

unsigned char* VTFCubeConverter::GetSourcePixel(int x, int y, int cubeFace)
{
    auto* faceVTF = &faceVTFs[cubeFace];
    int width = faceVTF->GetWidth();
    int height = faceVTF->GetHeight();

    x = min(width - 1, x);
    y = min(height - 1, y);

    auto vtfData = faceVTF->GetData(0, 0, 0, 0);
    return GetPixel(x, y, width, height, vtfData);
}

void VTFCubeConverter::SetTargetPixel(int x, int y, void* colour)
{
    float newPixelData[3];
    bgra2float(&newPixelData[0], &newPixelData[1], &newPixelData[2], static_cast<unsigned char *>(colour));
    ilSetPixels(x, y, 0, 1, 1, 1, IL_RGB, IL_FLOAT, newPixelData);
}

void VTFCubeConverter::GetCubeFaceSize(int cubeFace, int* width, int* height)
{
    auto face = &faceVTFs[cubeFace];
    *width = face->GetWidth();
    *height = face->GetHeight();

    //Pretend half height cube faces are square
    if (*height * 2 == *width)
    {
        *height = *width;
    }
}

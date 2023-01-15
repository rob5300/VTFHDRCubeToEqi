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
            int width = (int)faceVTFs[i].GetWidth();
            int height = (int)faceVTFs[i].GetHeight();
            maxSize = max(maxSize, width);
            maxSize = max(maxSize, height);

            //Decode vtf data from encodeded hdr bgra
            float* decodedData = new float[width * height * 3];
            decodedFaces[i] = decodedData;
            auto vtfData = faceVTFs[i].GetData(0, 0, 0, 0);
            int32_t t = 0;
            for (int32_t s = 0; s < width * height * 4; s+= 4, t += 3)
            {
                bgra2float(&vtfData[s], &decodedData[t]);
            }
        }
    }

    DoConvertion(maxSize);
}

float* VTFCubeConverter::GetSourcePixel(int x, int y, int cubeFace)
{
    auto* faceVTF = &faceVTFs[cubeFace];
    int width = faceVTF->GetWidth();
    int height = faceVTF->GetHeight();

    x = min(width - 1, x);
    y = min(height - 1, y);

    int start = ((width * y) + x) * 3;
    return &decodedFaces[cubeFace][start];
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

VTFCubeConverter::~VTFCubeConverter()
{
    for (int i = 0; i < 6; i++)
    {
        delete[] decodedFaces[i];
    }
}

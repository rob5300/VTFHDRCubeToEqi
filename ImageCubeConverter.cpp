#define _USE_MATH_DEFINES
#include <math.h>
#include "ImageCubeConverter.h"
#include <iostream>

using namespace std;

ImageCubeConverter::ImageCubeConverter(std::string path, ConvertOptions* options) : CubeConverter(path, options)
{
}

//https://www.graphics.cornell.edu/~bjw/rgbe/rgbe.c
static inline void rgbe2float(float *red, float *green, float *blue, float rgbe[4])
{
  if (rgbe[3]) {   /*nonzero pixel*/
    float f = ldexp(1.0,rgbe[3]-(int)(128+8));
    *red = rgbe[0] * f;
    *green = rgbe[1] * f;
    *blue = rgbe[2] * f;
  }
  else
    *red = *green = *blue = 0.0;
}

inline void rgba2float(float* input, float* output)
{
    rgbe2float(&output[0], &output[1], &output[2], input);
}

inline void rgba2float_valve(float* input, float* output)
{
    unsigned char r = input[0] * 255;
    unsigned char g = input[1] * 255;
    unsigned char b = input[2] * 255;
    unsigned char a = input[3] * 255;
    rgba2float_valve(&r, &g, &b, &a, output);
}

bool ImageCubeConverter::Convert(std::string* faces)
{
    int maxSize = 0;
    ilGenImages(6, faceIds);
    for (int i = 0; i < 6; i++)
    {
        ilBindImage(faceIds[i]);
        ilActiveImage(faceIds[i]);
        ilActiveLayer(0);
        ilLoadImage(faces[i].c_str());
        int width = ilGetInteger(IL_IMAGE_WIDTH);
        int height = ilGetInteger(IL_IMAGE_HEIGHT);

        if (width == 0 || height == 0)
        {
            printf("Image '%s' has invalid sizing of 0\n", faces[i].c_str());
            return false;
        }

        maxSize = max(maxSize, width);
        maxSize = max(maxSize, height);

        //Decode image, then de load it
        decodedFaces[i] = new DecodedImage(width, height);
        float* decodedFacePixels = decodedFaces[i]->pixels;
        float getPixelBuffer[4];
        
        printf("Loading image '%s'...\n", faces[i].c_str());
        for (int _y = 0; _y < height; _y++)
        {
            for (int _x = 0; _x < width; _x++)
            {
                int32_t decodedIndex = ((width * _y) + _x) * 3;
                ilCopyPixels(_x, _y, 0, 1, 1, 1, IL_RGBA, IL_FLOAT, getPixelBuffer);
                getPixelBuffer[3] = min(getPixelBuffer[3] + options->exposureShift, 1.0f);
                rgba2float_valve(getPixelBuffer, &decodedFacePixels[decodedIndex]);
            }
        }
    }
    ilDeleteImages(6, faceIds);

    DoConvertion(maxSize);

    return true;
}

void ImageCubeConverter::GetCubeFaceSize(int cubeFace, int* width, int* height)
{
    auto faceData = decodedFaces[cubeFace];
    *width = faceData->width;
    *height = faceData->height;
}

float* ImageCubeConverter::GetSourcePixel(int x, int y, int cubeFace)
{
    auto faceData = decodedFaces[cubeFace];

    x = min(faceData->width - 1, x);
    y = min(faceData->height - 1, y);

    int start = ((faceData->width * y) + x) * 3;
    return &(faceData->pixels[start]);
}

ImageCubeConverter::~ImageCubeConverter()
{
    delete[] decodedFaces;
}



#define _USE_MATH_DEFINES
#include <math.h>
#include "VTFLib.h"
#include "CubeConverter.h"
#include "CubemapFaces.h"
#include "ConvertOptions.h"

using namespace VTFLib;
using namespace std;

string CubeConverter::CubemapFaceNames[6] = {
	"up.vtf", "lf.vtf", "ft.vtf", "rt.vtf", "bk.vtf", "dn.vtf"
};

inline void OffsetTargetXY(int& x, int& y, int width, int height, ConvertOptions* options)
{
    x += (width * options->xOffset);
    if (x >= width)
    {
        x -= width;
    }
    else if (x < 0)
    {
        x += width;
    }

    y += (height * options->yOffset);
    if (y >= height)
    {
        y -= height;
    }
    else if (y < 0)
    {
        y += height;
    }
}

CubeConverter::CubeConverter(std::string path, ConvertOptions* options)
{
	this->path = path;
	this->options = options;
}

bool CubeConverter::Convert(std::string* faces)
{
    printf("Unsupported operation! Unimplemented converter type!");
    exit(1);
}

//Ported from: https://github.com/Mapiarz/CubemapToEquirectangular
ILuint CubeConverter::DoConvertion(int maxCubeFaceSize)
{
	targetImageId = ilGenImage();
    ILuint outputWidth = maxCubeFaceSize * 4;
    outputWidth *= options->scale;
    ILuint outputHeight = outputWidth / 2;
    printf("Creating hdr with dimensions %i x %i...\n", outputWidth, outputHeight);
    
    ActivateTargetImage();
    
    //Create image in devIL
    CreateNewImage(outputWidth, outputHeight);

    auto data = ilGetData();

    float u, v; //Normalised texture coordinates, from 0 to 1, starting at lower left corner
    float phi, theta; //Polar coordinates
    
    for (int j = 0; j < outputHeight; j++)
    {
        //Rows start from the bottom
        v = 1 - ((float)j / (float)outputHeight);
        theta = v * M_PI;

        for (int i = 0; i < outputWidth; i++)
        {
            //Columns start from the left
            u = ((float)i / (float)outputWidth);
            phi = u * 2.0 * M_PI;

            float x, y, z; //Unit vector
            x = sin(phi) * sin(theta) * -1.0;
            y = cos(theta);
            z = cos(phi) * sin(theta) * -1.0;

            float xa, ya, za;
            float a = max(abs(x), max(abs(y), abs(z)));

            //Vector Parallel to the unit vector that lies on one of the cube faces
            xa = x / a;
            ya = y / a;
            za = z / a;

            vlByte* color;
            float xPixel, yPixel;
            int cubeFace = -1;

            if (j == outputHeight / 2 && i == outputWidth / 2)
            {
                printf("");
            }

            if (xa == 1)
            {
                //Right
                xPixel = (((za + 1.0) / 2.0) - 1.0);
                yPixel = (((ya + 1.0) / 2.0));
                yPixel = 1 - yPixel;
                cubeFace = RIGHT;
            }
            else if (xa == -1)
            {
                //Left
                xPixel = (((za + 1.0) / 2.0));
                yPixel = (((ya + 1.0) / 2.0));
                yPixel = 1 - yPixel;
                cubeFace = LEFT;
            }
            else if (ya == 1)
            {
                //Up
                //x and y changed to rotate + flip img
                yPixel = (((xa + 1.0) / 2.0));
                xPixel = 1.0 - (((za + 1.0) / 2.0));
                cubeFace = TOP;
            }
            else if (ya == -1)
            {
                //Down
                xPixel = (((xa + 1.0) / 2.0));
                yPixel = (((za + 1.0) / 2.0));
                cubeFace = BOTTOM;
            }
            else if (za == 1)
            {
                //Front
                xPixel = (((xa + 1.0) / 2.0));
                yPixel = (((ya + 1.0) / 2.0));
                yPixel = 1 - yPixel;
                cubeFace = BACK;
            }
            else if (za == -1)
            {
                //Back
                xPixel = (((xa + 1.0) / 2.0) - 1.0);
                yPixel = (((ya + 1.0) / 2.0));
                yPixel = 1 - yPixel;
                cubeFace = FORWARD;
            }
            else
            {
                printf("Unknown face, something went wrong\n");
                xPixel = 0.0;
                yPixel = 0.0;
                //xOffset = 0;
                //yOffset = 0;
                cubeFace = -1;
            }

            xPixel = abs(xPixel);
            yPixel = abs(yPixel);

            //Get pixel from cube face tex and set in main texture
            if (cubeFace >= 0 && cubeFace < 6)
            {
                float color[3];
                int cubeFaceWidth;
                int cubeFaceHeight;
                GetCubeFaceSize(cubeFace, &cubeFaceWidth, &cubeFaceHeight);
                GetSourcePixelBilinear(xPixel, yPixel, cubeFace, cubeFaceWidth, cubeFaceHeight, color);

                OffsetTargetXY(i, j, outputWidth, outputHeight, options);
                SetTargetPixel(i, j, color);
            }
            else
            {
                printf("Invalid cube face use during conversion\n");
                return UINT_MAX;
            }
        }
    }

    return targetImageId;
}

void CubeConverter::CreateNewImage(int width, int height)
{
    //Define new image presuming source images are BGRA8888.
    ilTexImage(width, height, 1, 3, IL_RGB, IL_FLOAT, NULL);
}

void CubeConverter::ActivateTargetImage()
{
    ilBindImage(targetImageId);
    ilActiveImage(targetImageId);
    ilActiveLayer(0);
}

void CubeConverter::SetTargetPixel(int x, int y, float* colour)
{
    ilSetPixels(x, y, 0, 1, 1, 1, IL_RGB, IL_FLOAT, colour);
}

int CubeConverter::FindFaceForFilename(std::string &filename)
{
	for (int i = 0; i < 6; i++)
	{
        std::string* ending = &CubemapFaceNames[i];
		if (equal(ending->rbegin(), ending->rend(), filename.rbegin()))
		{
			return i;
		}
	}
	return -1;
}

void CubeConverter::FindCubemapFacesInFolder(std::string &folder, std::string* cubemapFacesPaths)
{
    for (const auto& entry : std::filesystem::directory_iterator(folder))
    {
        if (entry.is_regular_file())
        {
            std::string filename = entry.path().filename().string();
            int face = FindFaceForFilename(filename);
			if (face != -1)
			{
				cubemapFacesPaths[face] = entry.path().string();
			}
        }
    }
}

//Decode bgra to floating point HDR (https://developer.valvesoftware.com/wiki/Valve_Texture_Format)
void CubeConverter::bgra2float(unsigned char* bgra_input, float* rgb_output)
{
    const int ratio = 262144;
    if (bgra_input[3]) {
        float a = bgra_input[3] * 16;
        rgb_output[0] = (bgra_input[2] * a) / ratio;
        rgb_output[1] = (bgra_input[1] * a) / ratio;
        rgb_output[2] = (bgra_input[0] * a) / ratio;
    }
    else
        rgb_output[0] = rgb_output[1] = rgb_output[2] = 0.0;
}

//Get source pixel but perform bilinear interpolation
void CubeConverter::GetSourcePixelBilinear(float x, float y, int& cubeFace, int& width, int& height, float* result)
{
    x *= width;
    y *= height;

    int gxi = int(x);
    int gyi = int(y);

    float* c00 = GetSourcePixel(gxi, gyi, cubeFace);

    float* c10 = GetSourcePixel(gxi + 1, gyi, cubeFace);

    float* c01 = GetSourcePixel(gxi, gyi + 1, cubeFace);

    float* c11 = GetSourcePixel(gxi + 1, gyi + 1, cubeFace);

    x = x - gxi;
    y = y - gyi;

    float a[3];
    float b[3];

    for (int i = 0; i < 3; i++)
    {
        a[i] = c00[i] * (1.f - x) + c10[i] * x;
        b[i] = c01[i] * (1.f - x) + c11[i] * x;
    }

    for (int i = 0; i < 3; i++)
    {
        result[i] = a[i] * (1.f - y) + b[i] * y;
    }
}
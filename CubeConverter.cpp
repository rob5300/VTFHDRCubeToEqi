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

inline void OffsetTargetXY(int x, int y, int height, int width, ConvertOptions& options)
{
    x += (width * options.xOffset);
    if (x > width)
    {
        x -= width;
    }
    else if (x < 0)
    {
        x += width;
    }

    y += (height * options.yOffset);
    if (y > height)
    {
        y -= height;
    }
    else if (y < 0)
    {
        y += height;
    }
}

CubeConverter::CubeConverter(std::string path, ConvertOptions options)
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
int CubeConverter::DoConvertion(int maxCubeFaceSize)
{
	ILuint equiTexture = ilGenImage();
    ILuint outputWidth = maxCubeFaceSize * 4;
    ILuint outputHeight = outputWidth / 2;
    printf("Creating hdr with dimensions %i x %i...\n", outputWidth, outputHeight);
    ilBindImage(equiTexture);
    ilActiveImage(equiTexture);
    ilActiveLayer(0);
    
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
            OffsetTargetXY(i, j, outputWidth, outputHeight, options);

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

            //Clamp pixels to avoid border pixels
            xPixel = min(max(0.001f, xPixel), 0.999f);
            yPixel = min(max(0.001f, yPixel), 0.999f);

            //Get pixel from cube face tex and set in main texture
            if (cubeFace >= 0 && cubeFace < 6)
            {
                auto color = GetSourcePixel(xPixel, yPixel, cubeFace);

                if (color != nullptr)
                {
                    SetTargetPixel(i, j, color);
                }
                else
                {
                    printf("Failed to get valid colour from face '%i' at pos %f, %f\n", cubeFace, xPixel, yPixel);
                    return -1;
                }
            }
            else
            {
                printf("Invalid cube face use during conversion\n");
                return -1;
            }
        }
    }

    return equiTexture;
}

void CubeConverter::CreateNewImage(int width, int height)
{
    //Define new image presuming source images are BGRA8888.
    ilTexImage(width, height, 1, 3, IL_RGB, IL_FLOAT, NULL);
}

unsigned char* CubeConverter::GetSourcePixel(float x, float y, int cubeFace)
{
	return nullptr;
}

void CubeConverter::SetTargetPixel(int x, int y, void* colour)
{

}

//Get pointer to start of pixel data for this specific image
vlByte* CubeConverter::GetPixel(int& x, int& y, vlUInt& width, vlUInt& height, vlByte* data)
{
    int start = ((width * y) + x) * 4;
    return &data[start];
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
void CubeConverter::bgra2float(float *red, float *green, float *blue, unsigned char bgra[4])
{
    const int ratio = 262144;
    if (bgra[3]) {
        float a = bgra[3] * 16;
        *red = (bgra[2] * a) / ratio;
        *green = (bgra[1] * a) / ratio;
        *blue = (bgra[0] * a) / ratio;
    }
    else
        *red = *green = *blue = 0.0;
}
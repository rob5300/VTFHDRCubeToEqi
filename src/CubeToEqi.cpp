#include <windows.h>
#include <wtypes.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <string>
#include <filesystem>
#include "IL.h"
#include "VTFLib.h"

using namespace VTFLib;
using namespace std;

enum CubemapFaces
{
	TOP,
	LEFT,
	FORWARD,
	RIGHT,
	BACK,
	BOTTOM
};

string CubemapFaceNames[6] = {
	"up.vtf", "lf.vtf", "ft.vtf", "rt.vtf", "bk.vtf", "dn.vtf"
};

//Get pointer to start of pixel data for this specific image
inline vlByte* GetPixel(int& x, int& y, vlUInt& width, vlUInt& height, vlByte* data)
{
    int start = ((width * y) + x) * 4;
    return &data[start];
}

int FindFaceForFilename(string &filename)
{
	for (int i = 0; i < 6; i++)
	{
		string* ending = &CubemapFaceNames[i];
		if (equal(ending->rbegin(), ending->rend(), filename.rbegin()))
		{
			return i;
		}
	}
	return -1;
}

void FindCubemapFacesInFolder(string &folder, string* cubemapFacesPaths)
{
    for (const auto& entry : filesystem::directory_iterator(folder))
    {
        if (entry.is_regular_file() && entry.path().extension().string().find("vtf") != string::npos)
        {
            string filename = entry.path().filename().string();
            int face = FindFaceForFilename(filename);
			if (face != -1)
			{
				cubemapFacesPaths[face] = entry.path().string();
			}
        }
    }
}

//Decode bgra to floating point HDR (https://developer.valvesoftware.com/wiki/Valve_Texture_Format)
static void inline bgra2float(float *red, float *green, float *blue, unsigned char bgra[4])
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

//Ported from: https://github.com/Mapiarz/CubemapToEquirectangular
bool CreateEquirectangularImage(string* faces, string &folder)
{
    CVTFFile faceVTFs[6];
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
            maxSize = max(maxSize, faceVTFs[i].GetWidth());
            maxSize = max(maxSize, faceVTFs[i].GetHeight());
        }
    }
    
    ILuint equiTexture = ilGenImage();
    ILuint outputWidth = maxSize * 4;
    ILuint outputHeight = outputWidth / 2;
    printf("Will create hdr with dimensions %i x %i\n", outputWidth, outputHeight);
    ilBindImage(equiTexture);
    ilActiveImage(equiTexture);
    ilActiveLayer(0);
    //Define new image presuming source images are BGRA8888.
    ilTexImage(outputWidth, outputHeight, 1, 3, IL_RGB, IL_FLOAT, NULL);

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

            //Clamp pixels to avoid border pixels
            xPixel = min(max(0.001, xPixel), 0.999);
            yPixel = min(max(0.001, yPixel), 0.999);

            //Get pixel from cube face tex and set in main texture
            if (cubeFace >= 0 && cubeFace < 6)
            {
                auto* faceVTF = &faceVTFs[cubeFace];
                auto width = faceVTF->GetWidth();
                auto height = faceVTF->GetHeight();

                //Handle rectangular faces as if they were square and clamp y to bottom.
                bool halfHorizon = height == width * 0.5;
                if (halfHorizon)
                {
                    height = width;
                    yPixel = min(yPixel, 0.5);
                }

                int realX = xPixel * width;
                int realY = yPixel * height;

                if (halfHorizon)
                {
                    realY = min(faceVTF->GetHeight() - 1, realY);
                }

                //printf("* Set pixel (%i, %i) from. Source pixel (%i, %i) from face '%s'\n", i, j, (int)(xPixel * width), (int)(yPixel * height), CubemapFaceNames[cubeFace].c_str());
                auto vtfData = faceVTF->GetData(0, 0, 0, 0);
                color = GetPixel(realX, realY, width, height, vtfData);
                if (color != nullptr)
                {
                    float newPixelData[3];
                    bgra2float(&newPixelData[0], &newPixelData[1], &newPixelData[2], color);
                    ilSetPixels(i, j, 0, 1, 1, 1, IL_RGB, IL_FLOAT, newPixelData);
                }
                else
                {
                    printf("Failed to get valid colour from face '%i' at pos %f, %f\n", cubeFace, xPixel, yPixel);
                    return false;
                }
            }
            else
            {
                printf("Invalid cube face use during conversion\n");
                return false;
            }
        }
    }

    char savePath[512];
    strcpy_s(savePath, folder.c_str());
    strcat_s(savePath, "\\cubemap_hdr.hdr");
    if (filesystem::exists(savePath))
    {
        filesystem::remove(savePath);
    }
    ilSave(IL_HDR, savePath);
    printf("Saved hdr image to '%s'\n", savePath);
}

bool ConvertCubemapToEquirectangular(string cubemapFaceFolder)
{
    string faces[6];
    FindCubemapFacesInFolder(cubemapFaceFolder, faces);
    //Verify faces
    for (int i = 0; i < 6; i++)
    {
        if (faces[i].length() == 0)
        {
            printf("Failed to find all 6 faces of cubemap.");
            return false;
        }
    }

    return CreateEquirectangularImage(faces, cubemapFaceFolder);
}

int main(int argc, char *argv[])
{
    cout << "Cube to Equi converter by rob5300.\n";
    string targetFolder;
    if (argc > 1)
    {
        targetFolder = string(argv[1]);
    }
    else
    {
        cout << "Enter target folder:\n";
        getline(cin, targetFolder);
    }

    if (filesystem::is_directory(targetFolder))
    {
        ilInit();
        ConvertCubemapToEquirectangular(targetFolder);
    }
    else
    {
        printf("Error: '%s' is not a directory.", targetFolder);
    }
}
#pragma once
#include <string>
#include <filesystem>
#include "VTFLib.h"
#include "ConvertOptions.h"
#include "IL.h"

//Decode rgba to floating point HDR (https://developer.valvesoftware.com/wiki/Valve_Texture_Format)
inline void rgba2float_valve(unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a, float* rgb_output)
{
    const int ratio = 262144;
    if (a) {
        float _a = *a * 16;
        rgb_output[0] = (*r * _a) / ratio;
        rgb_output[1] = (*g * _a) / ratio;
        rgb_output[2] = (*b * _a) / ratio;
    }
    else
        rgb_output[0] = rgb_output[1] = rgb_output[2] = 0.0;
}

class CubeConverter
{
public:
	CubeConverter(std::string path, ConvertOptions* options);
	//Begin Conversion
	virtual bool Convert(std::string* faces);

	static int FindFaceForFilename(std::string& filename);
	static void FindCubemapFacesInFolder(std::string& folder, std::string* cubemapFacesPaths);
	static void bgra2float(unsigned char* bgra_input, float* rgb_output);
	static std::string CubemapFaceNames[6];

protected:
	//Source file/files path
	std::string path;
	//Conversion options
	ConvertOptions* options;
	//Target hdr image id
	ILuint targetImageId;
	//Start main conversion loop
	ILuint DoConvertion(int maxCubeFaceSize);
	//Create new target image in DevIL
	virtual void CreateNewImage(int width, int height);
	//Get colour from source cube face at pos
	virtual float* GetSourcePixel(int x, int y, int cubeFace) = 0;
	virtual void GetCubeFaceSize(int cubeFace, int* width, int* height) = 0;
	//Set new colour on target image at pos
	virtual void SetTargetPixel(int x, int y, float* colour);
	void ActivateTargetImage();

private:
	void GetSourcePixelBilinear(float x, float y, int& cubeFace, int& width, int& height, float* result);
};
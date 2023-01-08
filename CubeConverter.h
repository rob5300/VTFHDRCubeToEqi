#pragma once
#include <string>
#include <filesystem>
#include "VTFLib.h"
#include "ConvertOptions.h"
#include "IL.h"

class CubeConverter
{
public:
	CubeConverter(std::string path, ConvertOptions options);
	//Begin Conversion
	virtual bool Convert(std::string* faces);

	static vlByte* GetPixel(int& x, int& y, vlUInt& width, vlUInt& height, vlByte* data);
	static int FindFaceForFilename(std::string& filename);
	static void FindCubemapFacesInFolder(std::string& folder, std::string* cubemapFacesPaths);
	static void bgra2float(float* red, float* green, float* blue, unsigned char bgra[4]);
	static std::string CubemapFaceNames[6];

protected:
	//Source file/files path
	std::string path;
	//Conversion options
	ConvertOptions options;
	//Start main conversion loop
	int DoConvertion(int maxCubeFaceSize);
	//Create new target image in DevIL
	virtual void CreateNewImage(int width, int height);
	//Get colour from source cube face at pos
	virtual unsigned char* GetSourcePixel(float x, float y, int cubeFace);
	//Set new colour on target image at pos
	virtual void SetTargetPixel(int x, int y, void* colour);
};
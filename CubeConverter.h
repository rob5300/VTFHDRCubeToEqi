#pragma once
#include <string>
#include <filesystem>
#include "VTFLib.h"
#include "ConvertOptions.h"
#include "IL.h"

class CubeConverter
{
public:
	CubeConverter(std::string path, ConvertOptions* options);
	//Begin Conversion
	virtual bool Convert(std::string* faces);

	static vlByte* GetPixel(int& x, int& y, int& width, int& height, vlByte* data);
	static int FindFaceForFilename(std::string& filename);
	static void FindCubemapFacesInFolder(std::string& folder, std::string* cubemapFacesPaths);
	static void bgra2float(float* red, float* green, float* blue, unsigned char bgra[4]);
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
	virtual unsigned char* GetSourcePixel(int x, int y, int cubeFace);
	virtual void GetCubeFaceSize(int cubeFace, int* width, int* height) = 0;
	//Set new colour on target image at pos
	virtual void SetTargetPixel(int x, int y, void* colour) = 0;
	void ActivateTargetImage();

private:
	void GetSourcePixelBilinear(float x, float y, int& cubeFace, int& width, int& height, unsigned char result[3]);
};
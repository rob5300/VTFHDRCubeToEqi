#pragma once
#include "stdafx.h"
#include <string>



vlByte* GetPixel(int& x, int& y, int& width, int& height, vlByte* data);

int FindFaceForFilename(char* filename);

std::string* FindCubemapFacesInFolder(char* folder);

bool CreateEquirectangularImage(std::string*& faces, char*& folder);

bool ConvertCubemapToEquirectangular(char*& cubemapFaceFolder);
#pragma once
#include <string>

class ConvertOptions
{
public:
	float xOffset = 0.0f;
	float yOffset = 0.0f;
	float scale = 1.0f;
	bool HandleArgument(char* arg, char* data);
};
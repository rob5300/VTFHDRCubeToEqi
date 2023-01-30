#include "ConvertOptions.h"
#include <cassert>

inline void ArgHelp()
{
    printf("Argument Help:\n");
    printf(" -xoffset (0-1), -yoffset (0-1): How much to offset the output image by.\n");
    printf(" -scale 0<: Scale adjustment for output hdr. Default is 1\n");
    printf(" -exposureshift: Adjust exposure by this decimal value (non VTF only). Default is 0.0\n");
    printf(" -help: Show this help info.\n");
}

bool ConvertOptions::HandleArgument(char* arg, char* data)
{
	if (strcmp(arg, "-xoffset") == 0)
	{
		assert(data != nullptr, "xoffset did not have data");
		xOffset = std::stof(data);
		printf("xoffset set to '%f'\n", xOffset);
		return true;
	}
	else if (strcmp(arg, "-yoffset") == 0)
	{
		assert(data != nullptr, "yoffset did not have data");
		yOffset = std::stof(data);
		printf("yoffset set to '%f'\n", yOffset);
		return true;
	}
	else if (strcmp(arg, "-scale") == 0)
	{
		assert(data != nullptr, "Scale did not have data");
		scale = std::stof(data);
		if (scale == 0)
		{
			printf("Scale cannot be 0.\n");
			scale = 1;
		}
		else
		{
			printf("scale set to '%f'\n", scale);
		}
		return true;
	}
	else if (strcmp(arg, "-help") == 0)
	{
		ArgHelp();
		exit(0);
	}
	else if (strcmp(arg, "-exposureshift") == 0)
	{
		assert(data != nullptr, "exposureshift did not have data");
		exposureShift = std::stof(data);
		printf("Exposure shift set to '%f'\n", exposureShift);
		return true;
	}
	else
	{
		printf("Unknown argument '%s'.\n", arg);
		exit(1);
	}

	return false;
}

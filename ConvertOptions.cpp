#include "ConvertOptions.h"
#include <cassert>

inline void ArgHelp()
{
    printf("Argument Help:\n");
    printf(" -xoffset (0-1), -yoffset (0-1): How much to offset the output image by.\n");
    printf(" -help: Show this help info.\n");
}

bool ConvertOptions::HandleArgument(char* arg, char* data)
{
	if (strcmp(arg, "-xoffset") == 0)
	{
		assert(data != nullptr, "xoffset did not have data");
		xOffset = std::stof(data);
		return true;
	}
	else if (strcmp(arg, "-yoffset") == 0)
	{
		assert(data != nullptr, "yoffset did not have data");
		yOffset = std::stof(data);
		return true;
	}
	else if (strcmp(arg, "-help") == 0)
	{
		ArgHelp();
		exit(0);
	}
	else
	{
		printf("Unknown argument '%s'.", arg);
		exit(1);
	}

	return false;
}

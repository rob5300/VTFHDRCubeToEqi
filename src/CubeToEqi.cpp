#include <stdio.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "IL.h"
#include "..\ConvertOptions.h"
#include "..\CubeConverter.h"
#include "..\ImageCubeConverter.h"
#include "..\VTFCubeConverter.h"

using namespace std;

bool ConvertCubemapToEquirectangular(string cubemapFaceFolder, ConvertOptions* options)
{
    string faces[6];
    CubeConverter::FindCubemapFacesInFolder(cubemapFaceFolder, faces);
    CubeConverter* converter;
    filesystem::path firstFilePath = filesystem::path(faces[0]);
    auto extension = firstFilePath.extension().string();
    for (int i = 0; i < extension.length(); i++)
    {
        extension[i] = tolower(extension[i]);
    }
    if (extension == ".vtf"s)
    {
        converter = new VTFCubeConverter(cubemapFaceFolder, options);
    }
    else
    {
        converter = new ImageCubeConverter(cubemapFaceFolder, options);
    }

    ILuint createdImageId = converter->Convert(faces);
    delete converter;
    converter = nullptr;
    if (createdImageId != UINT_MAX)
    {
        //Create new filename to be the main name of the cubemap
        auto filename = filesystem::path(faces[0]).filename().string();
        filename.resize(filename.length() - 6);

        string savePath = cubemapFaceFolder + "\\"s + filename + ".hdr"s;
        if (filesystem::exists(savePath))
        {
            filesystem::remove(savePath);
        }
        ilSave(IL_HDR, savePath.c_str());
        //Delete image from DevIL
        ilDeleteImage(createdImageId);
        printf("Done! Saved hdr image to '%s'\n", savePath.c_str());

        return true;
    }

    return false;
}

int main(int argc, char *argv[])
{
    cout << "Cube to Equi converter by rob5300.\n";
    vector<string> targetFolders;
    ConvertOptions options;
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            char* arg = argv[i];
            if (arg[0] == '-')
            {
                //option arg found
                //Check if we can get data for this argument
                char* argData = i < argc - 1 ? argv[i + 1] : nullptr;
                if (options.HandleArgument(arg, argData))
                {
                    i++;
                }
            }
            else
            {
                targetFolders.push_back(string(arg));
            }
        }
    }
    else
    {
        cout << "Enter target folder:\n";
        targetFolders.resize(1);
        getline(cin, targetFolders[0]);
    }

    for (int i = 0; i < targetFolders.size(); i++)
    {
        auto folder = targetFolders[i];
        if (filesystem::is_directory(folder))
        {
            ilInit();
            printf("-> Input folder: '%s'.\n", folder.c_str());
            ConvertCubemapToEquirectangular(folder, &options);
        }
        else
        {
            printf("Error: '%s' is not a valid directory.\n", folder.c_str());
        }
    }
}
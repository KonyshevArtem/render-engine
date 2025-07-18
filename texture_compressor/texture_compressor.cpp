#include <string>
#include <iostream>

#include "texture_compressor_backend.h"
#include "arguments.h"

int main(int argc, char **argv)
{
    Arguments::Init(argv, argc);

    if (!Arguments::Contains("-output") ||
        !Arguments::Contains("-input") ||
        !Arguments::Contains("-platform"))
    {
        std::cout << "Parameters: -input -output -platform\n" << std::endl;
        return 0;
    }

    std::string outputPath = Arguments::Get("-output");
    std::string inputPath = Arguments::Get("-input");
    std::string platform = Arguments::Get("-platform");

    TextureCompressorBackend::CompressTextures(inputPath, outputPath, platform);

    return 0;
}
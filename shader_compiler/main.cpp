#include "shader_compiler.h"
#include "arguments.h"
#include "string_split.h"

#include <iostream>

int main(int argc, char **argv)
{
    Arguments::Init(argv, argc);

    if (!Arguments::Contains("-backend") || !Arguments::Contains("-output") || !Arguments::Contains("-input"))
    {
        std::cout << "No HLSL path or no target backend are specified" << std::endl;
        return 1;
    }

    const std::string backendName = Arguments::Get("-backend");
    const std::filesystem::path inputPath = Arguments::Get("-input");
	const std::filesystem::path outputPath = Arguments::Get("-output");
    const std::vector<std::string> defines = StringSplit::Split(Arguments::Get("-defines"), ',');
	const bool debug = Arguments::Contains("-debug");

	ShaderCompilerLib::CompileShader(inputPath, outputPath, backendName, defines, debug);

    return 0;
}
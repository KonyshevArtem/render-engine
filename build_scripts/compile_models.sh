if [ -z "$1" ]; then
    echo "Platform (windows, mac, ios, android): "; read PLATFORM
else
    PLATFORM=$1
fi

OS=$(uname)
if [ "$OS" = "Darwin" ]; then
    EXECUTABLE="../cmake-build-release-mac-arm64/model_compiler/ModelCompiler.app/Contents/MacOS/ModelCompiler"
else
    EXECUTABLE="../cmake-build-release-win64/model_compiler/Release/ModelCompiler.exe"
fi

INPUT_PATH="../core_resources/models"
OUTPUT_PATH="../build_resources/$PLATFORM/core_resources/models"

echo "Start compiling models to $OUTPUT_PATH"

Compile()
{
    inputPath="$INPUT_PATH/$1"
    $EXECUTABLE "-input" $inputPath "-output" $OUTPUT_PATH
}

Compile car.fbx
Compile cube.fbx
Compile cylinder.fbx
Compile plane.fbx
Compile sphere.fbx

echo "Finished compiling models to $OUTPUT_PATH";
if [ -z "$1" ]; then
    read _
fi
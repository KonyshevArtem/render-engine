if [ -z "$1" ]; then
    echo "Platform (windows, mac, ios, android): "; read PLATFORM
else
    PLATFORM=$1
fi

OS=$(uname)
if [ "$OS" = "Darwin" ]; then
    EXECUTABLE="../cmake-build-release-mac-arm64/font_compiler/FontCompiler.app/Contents/MacOS/FontCompiler"
else
    EXECUTABLE="../cmake-build-release-win64/font_compiler/Release/FontCompiler.exe"
fi

INPUT_PATH="../core_resources/fonts"
OUTPUT_PATH="../build_resources/$PLATFORM/core_resources/fonts"

echo "Start compiling fonts to $OUTPUT_PATH"

$EXECUTABLE "-input" $INPUT_PATH "-output" $OUTPUT_PATH

echo "Finished compiling fonts to $OUTPUT_PATH";
if [ -z "$1" ]; then
    read _
fi
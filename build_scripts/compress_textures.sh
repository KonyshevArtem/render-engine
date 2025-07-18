if [ -z "$1" ]; then
    echo "Platform (windows, mac, ios, android): "; read PLATFORM
else
    PLATFORM=$1
fi

OS=$(uname)
if [ "$OS" = "Darwin" ]; then
    EXECUTABLE="../cmake-build-release-mac-arm64/texture_compressor/TextureCompressor.app/Contents/MacOS/TextureCompressor"
else
    EXECUTABLE="../cmake-build-release-win64/texture_compressor/Release/TextureCompressor.exe"
fi

INPUT_PATH="../core_resources/textures"
OUTPUT_PATH="../build_resources/$PLATFORM/core_resources/textures"

echo "Start compressing textures for ${PLATFORM}"

$EXECUTABLE "-input" $INPUT_PATH "-output" $OUTPUT_PATH "-platform" $PLATFORM

echo "Finished compressing textures for ${PLATFORM}";
if [ -z "$1" ]; then
    read _
fi
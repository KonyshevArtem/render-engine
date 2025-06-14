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

if [ "$PLATFORM" = "windows" ]; then
    FORMAT="BC7"
elif [ "$PLATFORM" = "android" ]; then
    FORMAT="ASTC_6X6"
elif [ "$PLATFORM" = "mac" ]; then
    FORMAT="ASTC_6X6"
elif [ "$PLATFORM" = "ios" ]; then
    FORMAT="ASTC_6X6"
fi

INPUT_PATH="../core_resources/textures"
OUTPUT_PATH="../build_resources/$PLATFORM/core_resources/textures"

echo "Start compressing textures for ${PLATFORM} ${FORMAT}"

Compress()
{
    args=("-type" "$1" "-format" "$2" "-output" "$OUTPUT_PATH/$5" "-inputs" "$6")
    if [ $3 -gt 0 ]; then
        args+=("-linear")
    fi
    if [ $4 -gt 0 ]; then
        args+=("-mips")
    fi

    $EXECUTABLE "${args[@]}"
}

Compress Texture2D $FORMAT 0 1 car/car_albedo   $INPUT_PATH/car/car_albedo.tga
Compress Texture2D $FORMAT 1 1 car/car_data     $INPUT_PATH/car/car_data.tga
Compress Texture2D $FORMAT 1 1 car/car_normal   $INPUT_PATH/car/car_normal.tga

Compress Cubemap $FORMAT 0 1 skybox/skybox $INPUT_PATH/skybox/x_positive.png,$INPUT_PATH/skybox/x_negative.png,$INPUT_PATH/skybox/y_positive.png,$INPUT_PATH/skybox/y_negative.png,$INPUT_PATH/skybox/z_positive.png,$INPUT_PATH/skybox/z_negative.png\

Compress Texture2D $FORMAT 0 1 billboard_tree   $INPUT_PATH/billboard_tree.png
Compress Texture2D $FORMAT 0 1 brick            $INPUT_PATH/brick.png
Compress Texture2D $FORMAT 1 1 brick_normal     $INPUT_PATH/brick_normal.png
Compress Texture2D $FORMAT 0 1 water            $INPUT_PATH/water.png
Compress Texture2D $FORMAT 1 1 water_normal     $INPUT_PATH/water_normal.png
Compress Texture2D $FORMAT 0 1 window_cube      $INPUT_PATH/window_cube.png

echo "Finished compressing textures for ${PLATFORM} ${FORMAT}";
if [ -z "$1" ]; then
    read _
fi
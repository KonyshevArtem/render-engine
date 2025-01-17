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

$EXECUTABLE Texture2D $FORMAT 0 1 $OUTPUT_PATH/car/car_albedo   $INPUT_PATH/car/car_albedo.tga
$EXECUTABLE Texture2D $FORMAT 1 1 $OUTPUT_PATH/car/car_data     $INPUT_PATH/car/car_data.tga
$EXECUTABLE Texture2D $FORMAT 1 1 $OUTPUT_PATH/car/car_normal   $INPUT_PATH/car/car_normal.tga

$EXECUTABLE Cubemap $FORMAT 0 1 $OUTPUT_PATH/skybox/skybox $INPUT_PATH/skybox/x_positive.png\
                                                           $INPUT_PATH/skybox/x_negative.png\
                                                           $INPUT_PATH/skybox/y_positive.png\
                                                           $INPUT_PATH/skybox/y_negative.png\
                                                           $INPUT_PATH/skybox/z_positive.png\
                                                           $INPUT_PATH/skybox/z_negative.png\

$EXECUTABLE Texture2D $FORMAT 0 1 $OUTPUT_PATH/billboard_tree   $INPUT_PATH/billboard_tree.png
$EXECUTABLE Texture2D $FORMAT 0 1 $OUTPUT_PATH/brick            $INPUT_PATH/brick.png
$EXECUTABLE Texture2D $FORMAT 1 1 $OUTPUT_PATH/brick_normal     $INPUT_PATH/brick_normal.png
$EXECUTABLE Texture2D $FORMAT 0 1 $OUTPUT_PATH/water            $INPUT_PATH/water.png
$EXECUTABLE Texture2D $FORMAT 1 1 $OUTPUT_PATH/water_normal     $INPUT_PATH/water_normal.png
$EXECUTABLE Texture2D $FORMAT 0 1 $OUTPUT_PATH/window_cube      $INPUT_PATH/window_cube.png

echo "Finished compressing textures for ${PLATFORM} ${FORMAT}";
if [ -z "$1" ]; then
    read _
fi
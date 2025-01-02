read -p "Platform (windows, apple, android): " PLATFORM

OS=$(uname)
if [ "$OS" = "Darwin" ]; then
    EXECUTABLE="../cmake-build-release/texture_compressor/TextureCompressor.app/Contents/MacOS/TextureCompressor"
else
    EXECUTABLE="../cmake-build-release/texture_compressor/TextureCompressor.exe"
fi

if [ "$PLATFORM" = "windows" ]; then
    FORMAT="BC7"
elif [ "$PLATFORM" = "android" ]; then
    FORMAT="ASTC_6X6"
elif [ "$PLATFORM" = "apple" ]; then
    FORMAT="ASTC_6X6"
fi

BASE_PATH="../core_resources/textures"

$EXECUTABLE Texture2D $FORMAT 0 1 car_albedo   $BASE_PATH/car/car_albedo.tga
$EXECUTABLE Texture2D $FORMAT 1 1 car_data     $BASE_PATH/car/car_data.tga
$EXECUTABLE Texture2D $FORMAT 1 1 car_normal   $BASE_PATH/car/car_normal.tga

$EXECUTABLE Cubemap $FORMAT 0 1 skybox $BASE_PATH/skybox/x_positive.png\
                                       $BASE_PATH/skybox/x_negative.png\
                                       $BASE_PATH/skybox/y_positive.png\
                                       $BASE_PATH/skybox/y_negative.png\
                                       $BASE_PATH/skybox/z_positive.png\
                                       $BASE_PATH/skybox/z_negative.png\

$EXECUTABLE Texture2D $FORMAT 0 1 billboard_tree   $BASE_PATH/billboard_tree.png
$EXECUTABLE Texture2D $FORMAT 0 1 brick            $BASE_PATH/brick.png
$EXECUTABLE Texture2D $FORMAT 1 1 brick_normal     $BASE_PATH/brick_normal.png
$EXECUTABLE Texture2D $FORMAT 0 1 water            $BASE_PATH/water.png
$EXECUTABLE Texture2D $FORMAT 1 1 water_normal     $BASE_PATH/water_normal.png
$EXECUTABLE Texture2D $FORMAT 0 1 window_cube      $BASE_PATH/window_cube.png

read -p "Finished compressing textures for ${PLATFORM} ${FORMAT}" _
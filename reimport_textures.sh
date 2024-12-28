read -p "Platform (windows, apple, android): " PLATFORM

OS=$(uname)
if [ "$OS" = "Darwin" ]; then
    EXECUTABLE="./cmake-build-release/texture_compressor/TextureCompressor.app/Contents/MacOS/TextureCompressor"
else
    EXECUTABLE="./cmake-build-release/texture_compressor/TextureCompressor.exe"
fi

if [ "$PLATFORM" = "windows" ]; then
    FORMAT="BC7"
elif [ "$PLATFORM" = "android" ]; then
    FORMAT="ASTC_6X6"
elif [ "$PLATFORM" = "apple" ]; then
    FORMAT="ASTC_6X6"
fi

$EXECUTABLE Texture2D $FORMAT 0 1 car_albedo   core_resources/textures/car/car_albedo.tga
$EXECUTABLE Texture2D $FORMAT 1 1 car_data     core_resources/textures/car/car_data.tga
$EXECUTABLE Texture2D $FORMAT 1 1 car_normal   core_resources/textures/car/car_normal.tga

$EXECUTABLE Cubemap $FORMAT 0 1 skybox core_resources/textures/skybox/x_positive.png\
                                       core_resources/textures/skybox/x_negative.png\
                                       core_resources/textures/skybox/y_positive.png\
                                       core_resources/textures/skybox/y_negative.png\
                                       core_resources/textures/skybox/z_positive.png\
                                       core_resources/textures/skybox/z_negative.png\

$EXECUTABLE Texture2D $FORMAT 0 1 billboard_tree   core_resources/textures/billboard_tree.png
$EXECUTABLE Texture2D $FORMAT 0 1 brick            core_resources/textures/brick.png
$EXECUTABLE Texture2D $FORMAT 1 1 brick_normal     core_resources/textures/brick_normal.png
$EXECUTABLE Texture2D $FORMAT 0 1 water            core_resources/textures/water.png
$EXECUTABLE Texture2D $FORMAT 1 1 water_normal     core_resources/textures/water_normal.png
$EXECUTABLE Texture2D $FORMAT 0 1 window_cube      core_resources/textures/window_cube.png

read -p "Finished compressing textures for ${PLATFORM} ${FORMAT}" _
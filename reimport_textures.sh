OS=$(uname)
if [ "$OS" = "Darwin" ]; then
    EXECUTABLE="./cmake-build-debug/texture_compressor/TextureCompressor"
else
    EXECUTABLE="./cmake-build-debug-visual-studio/texture_compressor/TextureCompressor.exe"
fi

$EXECUTABLE Texture2D BC7 0 1 car_albedo   resources/textures/car/car_albedo.tga
$EXECUTABLE Texture2D BC7 1 1 car_data     resources/textures/car/car_data.tga
$EXECUTABLE Texture2D BC7 1 1 car_normal   resources/textures/car/car_normal.tga

$EXECUTABLE Cubemap BC7 0 1 skybox resources/textures/skybox/x_positive.png\
                                   resources/textures/skybox/x_negative.png\
                                   resources/textures/skybox/y_positive.png\
                                   resources/textures/skybox/y_negative.png\
                                   resources/textures/skybox/z_positive.png\
                                   resources/textures/skybox/z_negative.png\

$EXECUTABLE Texture2D BC7 0 1 billboard_tree   resources/textures/billboard_tree.png
$EXECUTABLE Texture2D BC7 0 1 brick            resources/textures/brick.png
$EXECUTABLE Texture2D BC7 1 1 brick_normal     resources/textures/brick_normal.png
$EXECUTABLE Texture2D BC7 0 1 water            resources/textures/water.png
$EXECUTABLE Texture2D BC7 1 1 water_normal     resources/textures/water_normal.png
$EXECUTABLE Texture2D BC7 0 1 window_cube      resources/textures/window_cube.png
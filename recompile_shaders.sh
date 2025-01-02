read -p "Platform (windows, apple, android): " PLATFORM

OS=$(uname)
if [ "$OS" = "Darwin" ]; then
    EXECUTABLE="./cmake-build-release/shader_compiler/ShaderCompiler.app/Contents/MacOS/ShaderCompiler"
else
    EXECUTABLE="./cmake-build-release/shader_compiler/ShaderCompiler.exe"
fi

if [ "$PLATFORM" = "windows" ]; then
    BACKEND="opengl"
elif [ "$PLATFORM" = "android" ]; then
    BACKEND="gles"
elif [ "$PLATFORM" = "apple" ]; then
    BACKEND="metal"
fi

$EXECUTABLE $BACKEND core_resources/shaders/final_blit.hlsl

$EXECUTABLE $BACKEND core_resources/shaders/fallback.hlsl

$EXECUTABLE $BACKEND core_resources/shaders/skybox.hlsl

$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _RECEIVE_SHADOWS
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _RECEIVE_SHADOWS _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _RECEIVE_SHADOWS _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _RECEIVE_SHADOWS _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _REFLECTION
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _REFLECTION _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _REFLECTION _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _REFLECTION _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _REFLECTION _RECEIVE_SHADOWS
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _REFLECTION _RECEIVE_SHADOWS _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _REFLECTION _RECEIVE_SHADOWS _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _REFLECTION _RECEIVE_SHADOWS _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _RECEIVE_SHADOWS
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _RECEIVE_SHADOWS _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _RECEIVE_SHADOWS _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _RECEIVE_SHADOWS _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _REFLECTION
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _REFLECTION _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _REFLECTION _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _REFLECTION _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _REFLECTION _RECEIVE_SHADOWS
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _REFLECTION _RECEIVE_SHADOWS _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _REFLECTION _RECEIVE_SHADOWS _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _NORMAL_MAP _REFLECTION _RECEIVE_SHADOWS _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _RECEIVE_SHADOWS
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _RECEIVE_SHADOWS _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _RECEIVE_SHADOWS _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _RECEIVE_SHADOWS _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _REFLECTION
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _REFLECTION _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _REFLECTION _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _REFLECTION _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _REFLECTION _RECEIVE_SHADOWS
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _REFLECTION _RECEIVE_SHADOWS _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _REFLECTION _RECEIVE_SHADOWS _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _REFLECTION _RECEIVE_SHADOWS _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _RECEIVE_SHADOWS
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _RECEIVE_SHADOWS _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _RECEIVE_SHADOWS _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _RECEIVE_SHADOWS _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _REFLECTION
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _REFLECTION _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _REFLECTION _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _REFLECTION _INSTANCING _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _REFLECTION _RECEIVE_SHADOWS
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _REFLECTION _RECEIVE_SHADOWS _PER_INSTANCE_DATA
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _REFLECTION _RECEIVE_SHADOWS _INSTANCING
$EXECUTABLE $BACKEND core_resources/shaders/standard.hlsl _DATA_MAP _NORMAL_MAP _REFLECTION _RECEIVE_SHADOWS _INSTANCING _PER_INSTANCE_DATA

$EXECUTABLE $BACKEND core_resources/shaders/shadowCaster.hlsl

$EXECUTABLE $BACKEND core_resources/shaders/outlineBlit.hlsl

$EXECUTABLE $BACKEND core_resources/shaders/silhouette.hlsl

$EXECUTABLE $BACKEND core_resources/shaders/billboard.hlsl

$EXECUTABLE $BACKEND core_resources/shaders/gizmos.hlsl _INSTANCING

$EXECUTABLE $BACKEND core_resources/shaders/editor/shadowMapDebug.hlsl

read -p "Finished compiling shaders for ${PLATFORM} ${BACKEND}" _
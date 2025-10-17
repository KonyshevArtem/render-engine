if [ -z "$1" ]; then
    echo "Platform (windows, mac, ios, android): "; read PLATFORM
else
    PLATFORM=$1
fi

OS=$(uname)
if [ "$OS" = "Darwin" ]; then
    EXECUTABLE="../cmake-build-release-mac-arm64/shader_compiler/ShaderCompiler.app/Contents/MacOS/ShaderCompiler"
else
    EXECUTABLE="../cmake-build-release-win64/shader_compiler/Release/ShaderCompiler.exe"
fi

if [ "$PLATFORM" = "windows" ]; then
    BACKENDS=("opengl" "dx12")
elif [ "$PLATFORM" = "android" ]; then
    BACKENDS=("gles")
elif [ "$PLATFORM" = "mac" ]; then
    BACKENDS=("metal")
elif [ "$PLATFORM" = "ios" ]; then
    BACKENDS=("metal")
fi

INPUT_PATH="../core_resources/shaders"
OUTPUT_PATH="../build_resources/$PLATFORM/core_resources/shaders"

echo "Start compiling shaders for ${PLATFORM} ${BACKENDS}"

Compile()
{
    outputPath="$OUTPUT_PATH/$1"
    inputPath="$INPUT_PATH/$1.hlsl"

    for backend in "${BACKENDS[@]}"; do
        $EXECUTABLE "-backend" $backend "-output" $outputPath "-input" $inputPath "-defines" $2
    done
}

Compile final_blit

Compile fallback

Compile skybox

Compile standard
Compile standard _INSTANCING
Compile standard _RECEIVE_SHADOWS
Compile standard _RECEIVE_SHADOWS,_INSTANCING
Compile standard _REFLECTION
Compile standard _REFLECTION,_INSTANCING
Compile standard _REFLECTION,_RECEIVE_SHADOWS
Compile standard _REFLECTION,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _NORMAL_MAP
Compile standard _NORMAL_MAP,_INSTANCING
Compile standard _NORMAL_MAP,_RECEIVE_SHADOWS
Compile standard _NORMAL_MAP,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _NORMAL_MAP,_REFLECTION
Compile standard _NORMAL_MAP,_REFLECTION,_INSTANCING
Compile standard _NORMAL_MAP,_REFLECTION,_RECEIVE_SHADOWS
Compile standard _NORMAL_MAP,_REFLECTION,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _DATA_MAP
Compile standard _DATA_MAP,_INSTANCING
Compile standard _DATA_MAP,_RECEIVE_SHADOWS
Compile standard _DATA_MAP,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _DATA_MAP,_REFLECTION
Compile standard _DATA_MAP,_REFLECTION,_INSTANCING
Compile standard _DATA_MAP,_REFLECTION,_RECEIVE_SHADOWS
Compile standard _DATA_MAP,_REFLECTION,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _DATA_MAP,_NORMAL_MAP
Compile standard _DATA_MAP,_NORMAL_MAP,_INSTANCING
Compile standard _DATA_MAP,_NORMAL_MAP,_RECEIVE_SHADOWS
Compile standard _DATA_MAP,_NORMAL_MAP,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _DATA_MAP,_NORMAL_MAP,_REFLECTION
Compile standard _DATA_MAP,_NORMAL_MAP,_REFLECTION,_INSTANCING
Compile standard _DATA_MAP,_NORMAL_MAP,_REFLECTION,_RECEIVE_SHADOWS
Compile standard _DATA_MAP,_NORMAL_MAP,_REFLECTION,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _ALPHA_CLIP
Compile standard _ALPHA_CLIP,_INSTANCING
Compile standard _ALPHA_CLIP,_RECEIVE_SHADOWS
Compile standard _ALPHA_CLIP,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _ALPHA_CLIP,_REFLECTION
Compile standard _ALPHA_CLIP,_REFLECTION,_INSTANCING
Compile standard _ALPHA_CLIP,_REFLECTION,_RECEIVE_SHADOWS
Compile standard _ALPHA_CLIP,_REFLECTION,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _ALPHA_CLIP,_NORMAL_MAP
Compile standard _ALPHA_CLIP,_NORMAL_MAP,_INSTANCING
Compile standard _ALPHA_CLIP,_NORMAL_MAP,_RECEIVE_SHADOWS
Compile standard _ALPHA_CLIP,_NORMAL_MAP,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _ALPHA_CLIP,_NORMAL_MAP,_REFLECTION
Compile standard _ALPHA_CLIP,_NORMAL_MAP,_REFLECTION,_INSTANCING
Compile standard _ALPHA_CLIP,_NORMAL_MAP,_REFLECTION,_RECEIVE_SHADOWS
Compile standard _ALPHA_CLIP,_NORMAL_MAP,_REFLECTION,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _ALPHA_CLIP,_DATA_MAP
Compile standard _ALPHA_CLIP,_DATA_MAP,_INSTANCING
Compile standard _ALPHA_CLIP,_DATA_MAP,_RECEIVE_SHADOWS
Compile standard _ALPHA_CLIP,_DATA_MAP,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _ALPHA_CLIP,_DATA_MAP,_REFLECTION
Compile standard _ALPHA_CLIP,_DATA_MAP,_REFLECTION,_INSTANCING
Compile standard _ALPHA_CLIP,_DATA_MAP,_REFLECTION,_RECEIVE_SHADOWS
Compile standard _ALPHA_CLIP,_DATA_MAP,_REFLECTION,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _ALPHA_CLIP,_DATA_MAP,_NORMAL_MAP
Compile standard _ALPHA_CLIP,_DATA_MAP,_NORMAL_MAP,_INSTANCING
Compile standard _ALPHA_CLIP,_DATA_MAP,_NORMAL_MAP,_RECEIVE_SHADOWS
Compile standard _ALPHA_CLIP,_DATA_MAP,_NORMAL_MAP,_RECEIVE_SHADOWS,_INSTANCING
Compile standard _ALPHA_CLIP,_DATA_MAP,_NORMAL_MAP,_REFLECTION
Compile standard _ALPHA_CLIP,_DATA_MAP,_NORMAL_MAP,_REFLECTION,_INSTANCING
Compile standard _ALPHA_CLIP,_DATA_MAP,_NORMAL_MAP,_REFLECTION,_RECEIVE_SHADOWS
Compile standard _ALPHA_CLIP,_DATA_MAP,_NORMAL_MAP,_REFLECTION,_RECEIVE_SHADOWS,_INSTANCING

Compile shadowCaster

Compile outlineBlit

Compile silhouette

Compile billboard

Compile gizmos _INSTANCING
Compile gizmos _INSTANCING,_FRUSTUM_GIZMO

Compile editor/shadowMapOverlay
Compile editor/shadowCascadeVisualize

Compile ui/image

echo "Finished compiling shaders for ${PLATFORM} ${BACKENDS}";
if [ -z "$1" ]; then
    read _
fi
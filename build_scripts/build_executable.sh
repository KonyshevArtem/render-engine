if [ -z "$1" ]; then
    echo "Platform (windows, mac, ios, android): "; read PLATFORM
else
    PLATFORM=$1
fi

OUTPUT_PATH_WIN64="../cmake-build-release-win64"
OUTPUT_PATH_ANDROID_ARM64="../cmake-build-relwithdebinfo-android-arm64"
OUTPUT_PATH_MAC_ARM64="../cmake-build-release-mac-arm64"
OUTPUT_PATH_IOS="../cmake-build-release-ios"

ANDROID_TOOLCHAIN_ARG=(-DCMAKE_TOOLCHAIN_FILE="${NDK_ROOT}/build/cmake/android.toolchain.cmake")
IOS_TOOLCHAIN_ARG=(-DCMAKE_TOOLCHAIN_FILE="../ios.toolchain.cmake")

NINJA_GENERATOR="Ninja"
VS_GENERATOR="Visual Studio 17 2022"
UNIX_MAKEFILES_GENERATOR="Unix Makefiles"

ANDROID_CMAKE_ARGS=(-DANDROID_ABI=arm64-v8a "${ANDROID_TOOLCHAIN_ARG[@]}")
MAC_CMAKE_ARGS=(-DPLATFORM=MAC_ARM64 -DDEPLOYMENT_TARGET=15.5 "${IOS_TOOLCHAIN_ARG[@]}")
IOS_CMAKE_ARGS=(-DPLATFORM=OS64 -DDEPLOYMENT_TARGET=16.3 "${IOS_TOOLCHAIN_ARG[@]}")

BUILD_TYPE="Release"

OS=$(uname)
if [ "$OS" = "Darwin" ]; then
    TOOLS_OUTPUT_PATH=${OUTPUT_PATH_MAC_ARM64}
    TOOLS_CMAKE_ARGS=${MAC_CMAKE_ARGS[@]}
    TOOLS_GENERATOR=${UNIX_MAKEFILES_GENERATOR}
else
    TOOLS_OUTPUT_PATH=${OUTPUT_PATH_WIN64}
    TOOLS_GENERATOR=${VS_GENERATOR}
fi

if [ "$PLATFORM" = "windows" ]; then
    OUTPUT_PATH=${OUTPUT_PATH_WIN64}
    GENERATOR=${VS_GENERATOR}
elif [ "$PLATFORM" = "android" ]; then
    OUTPUT_PATH=${OUTPUT_PATH_ANDROID_ARM64}
    CMAKE_ARGS=("${ANDROID_CMAKE_ARGS[@]}")
    GENERATOR=${NINJA_GENERATOR}
    BUILD_TYPE="RelWithDebInfo"
elif [ "$PLATFORM" = "mac" ]; then
    OUTPUT_PATH=${OUTPUT_PATH_MAC_ARM64}
    CMAKE_ARGS=("${MAC_CMAKE_ARGS[@]}")
    GENERATOR=${UNIX_MAKEFILES_GENERATOR}
elif [ "$PLATFORM" = "ios" ]; then
    OUTPUT_PATH=${OUTPUT_PATH_IOS}
    CMAKE_ARGS=("${IOS_CMAKE_ARGS[@]}")
    GENERATOR=${UNIX_MAKEFILES_GENERATOR}
fi

cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -G "${TOOLS_GENERATOR}" -S .. -B "${TOOLS_OUTPUT_PATH}" ${TOOLS_CMAKE_ARGS[@]}
cmake --build ${TOOLS_OUTPUT_PATH} --config ${BUILD_TYPE} --target ShaderCompiler
cmake --build ${TOOLS_OUTPUT_PATH} --config ${BUILD_TYPE} --target TextureCompressor

cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -G "${GENERATOR}" -S .. -B "${OUTPUT_PATH}" ${CMAKE_ARGS[@]}
cmake --build ${OUTPUT_PATH} --config ${BUILD_TYPE} --target RenderEngineLauncher

source build_resources.sh $PLATFORM

if [ "${PLATFORM}" = "android" ]; then
    ANDROID_PROJECT_PATH="../launchers/android_launcher/AndroidStudio"
    ${ANDROID_PROJECT_PATH}/gradlew -p "${ANDROID_PROJECT_PATH}" build
fi

echo "Render Engine Executable build finished for ${PLATFORM}"
read
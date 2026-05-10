if [ -z "$1" ]; then
    echo "Platform (windows, mac, ios, android): "; read PLATFORM
else
    PLATFORM=$1
fi

source generate_solutions.sh $PLATFORM

cmake --build ${TOOLS_OUTPUT_PATH} --config ${BUILD_TYPE} --target ShaderCompiler
cmake --build ${TOOLS_OUTPUT_PATH} --config ${BUILD_TYPE} --target TextureCompressor
cmake --build ${TOOLS_OUTPUT_PATH} --config ${BUILD_TYPE} --target ModelCompiler
cmake --build ${OUTPUT_PATH} --config ${BUILD_TYPE} --target RenderEngineLauncher

source build_resources.sh $PLATFORM

if [ "${PLATFORM}" = "android" ]; then
    ANDROID_PROJECT_PATH="../launchers/android_launcher/AndroidStudio"
    ${ANDROID_PROJECT_PATH}/gradlew -p "${ANDROID_PROJECT_PATH}" build
fi

echo "Render Engine Executable build finished for ${PLATFORM}"
read
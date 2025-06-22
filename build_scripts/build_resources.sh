if [ -z "$1" ]; then
    echo "Platform (windows, mac, ios, android): "; read PLATFORM
else
    PLATFORM=$1
fi

if [[ "$PLATFORM" != "windows" && "$PLATFORM" != "android" && "$PLATFORM" != "mac" && "$PLATFORM" != "ios" ]]; then
    echo "Unknown platform: $PLATFORM"; read _
    exit 1
fi

source compile_shaders.sh $PLATFORM
source compress_textures.sh $PLATFORM
source copy_models.sh $PLATFORM
source copy_scenes.sh $PLATFORM

echo "Finished building resources";
if [ -z "$1" ]; then
    read _
fi
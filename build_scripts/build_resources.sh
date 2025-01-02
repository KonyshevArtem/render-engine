echo "Platform (windows, apple, android): "; read PLATFORM

if [[ "$PLATFORM" != "windows" && "$PLATFORM" != "android" && "$PLATFORM" != "apple" ]]; then
    echo "Unknown platform: $PLATFORM"; read _
    exit 1
fi

source compile_shaders.sh $PLATFORM
source compress_textures.sh <<< $PLATFORM
source copy_models.sh <<< $PLATFORM

echo "Finished building resources"; read _
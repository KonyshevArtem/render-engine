read -p "Platform (windows, apple, android): " PLATFORM

if [[ "$PLATFORM" != "windows" && "$PLATFORM" != "android" && "$PLATFORM" != "apple" ]]; then
    read -p "Unknown platform: $PLATFORM" _
    exit 1
fi

source compile_shaders.sh <<< $PLATFORM
source compress_textures.sh <<< $PLATFORM
source copy_models.sh <<< $PLATFORM
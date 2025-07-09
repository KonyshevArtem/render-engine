if [ -z "$1" ]; then
    echo "Platform (windows, mac, ios, android): "; read PLATFORM
else
    PLATFORM=$1
fi

OUTPUT_PATH="../build_resources/$PLATFORM/core_resources"

echo "Start copying scenes to $OUTPUT_PATH"

mkdir -p $OUTPUT_PATH
cp -r -v ../core_resources/scenes $OUTPUT_PATH

echo "Finished copying models to $OUTPUT_PATH";
if [ -z "$1" ]; then
    read _
fi
if [ -z "$1" ]; then
    echo "Platform (windows, apple, android): "; read PLATFORM
else
    PLATFORM=$1
fi

OUTPUT_PATH="../build_resources/$PLATFORM/core_resources"

echo "Start copying models to $OUTPUT_PATH"

mkdir -p $OUTPUT_PATH
cp -r -v ../core_resources/models $OUTPUT_PATH

echo "Finished copying models to $OUTPUT_PATH"; read _
if [ -z "$1" ]; then
    read _
fi
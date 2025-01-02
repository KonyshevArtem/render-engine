read -p "Platform (windows, apple, android): " PLATFORM

OUTPUT_PATH="../build_resources/$PLATFORM/core_resources/models"

echo "Start copying models to $OUTPUT_PATH"

mkdir -p $OUTPUT_PATH
cp -r -T -v ../core_resources/models $OUTPUT_PATH

read -p "Finished copying models to $OUTPUT_PATH" _
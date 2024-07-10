mkdir build64Win
cd build64Win
# Run CMake to generate the build files
cmake ..

# Build the project
cmake --build .

# Source file path
SOURCE="/kafka/src/build64Win/lib1C_kafka.so"

# Destination file path
DESTINATION="/home/usr1cv8/lib1C_kafka.so"

cp $SOURCE $DESTINATION

if [ $? -eq 0 ]; then
    echo "File copied successfully."
else
    echo "Failed to copy the file."
fi

# copy to 1C
echo "done compiled"
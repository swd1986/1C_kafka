mkdir build64Win
cd build64Win
# Run CMake to generate the build files
cmake ..

# Build the project
cmake --build .

# copy to 1C
cp build64Win/lib1C_kafka.so /home/usr1cv8/
echo "file copied, done compiled"
# LINUX compile
g++ -fPIC -shared -o AddInNative.so AddInNative.cpp

# Windows compile
clang++ -fPIC -shared -o AddInNative.so AddInNative.cpp



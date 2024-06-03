#include <iostream>
#include <dlfcn.h>

typedef void (*FunctionPtr)();

int main() {

    void* handle = dlopen("../../bin/libNativeAddInLin64.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Error loading shared object: " << dlerror() << std::endl;
        return 1;
    }

    FunctionPtr myFunction = (FunctionPtr)dlsym(handle, "getVersion");
    if (!myFunction) {
        std::cerr << "Error finding function: " << dlerror() << std::endl;
        dlclose(handle);
        return 1;
    }

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
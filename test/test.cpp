#include <iostream>
#include <windows.h>

struct tVariant {
    // ...
};

typedef bool (*CallAsFunc)(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);

int main()
{

    HINSTANCE hDLL = LoadLibrary(L"d:/WORK/1C/1c_library_example/NativeAPI/bind64/AddInNativeWin64.dll");
    if (hDLL == NULL) {
        std::cerr << "Could not load the DLL." << std::endl;
        return 1;
    }

    // Get the function pointer
    CallAsFunc CF = (CallAsFunc)GetProcAddress(hDLL, "CallAsFunc");
    if (!CF) {
        std::cerr << "Could not locate the function." << std::endl;
        FreeLibrary(hDLL);
        return 1;
    }

    // Prepare parameters and call the function
    tVariant retValue;
    tVariant params[10]; // Example, depends on your actual use case
    long lMethodNum = 0; // Example method number
    long lSizeArray = sizeof(params) / sizeof(params[0]);

    bool result = CF(lMethodNum, &retValue, params, lSizeArray);
    if (result) {
        std::cout << "Function executed successfully." << std::endl;
    }
    else {
        std::cerr << "Function execution failed." << std::endl;
    }

    // Free the DLL
    FreeLibrary(hDLL);

    std::cout << "Done!\n";
}


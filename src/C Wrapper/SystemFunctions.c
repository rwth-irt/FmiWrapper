#include "SystemFunctions.h"

#if defined _WIN32
#include <windows.h>
#else
#if __unix__
#include <dlfcn.h>
#else
#define PUBLIC_EXPORT
#endif
#endif

void* loadSharedLibrary(const char *filename)
{

#if defined(_WIN32) // Microsoft compiler
    return (void*)LoadLibrary(filename);
#elif defined(__unix__) // GNU compiler
    return dlopen(filename, RTLD_NOW);
#endif

}

void *getFunction(void *handle, const char *function)
{
#if defined(_WIN32) // Microsoft compiler
    return (void*)GetProcAddress((HINSTANCE)handle, function);
#elif defined(__unix__) // GNU compiler
    return dlsym(handle, function);
#endif
}

int freeSharedLibrary(void *handle)
{
#if defined(_WIN32) // Microsoft compiler
    return FreeLibrary((HINSTANCE)handle);
#elif defined(__unix__) // GNU compiler
    return dlclose(handle);
#endif
}

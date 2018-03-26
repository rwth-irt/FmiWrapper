#pragma once

/*! 
    \brief Wrapper for platform specific functions.
*/


/*!
    Load the library into the current process. For security reasons a full path is recomended.
    \return The handle for the library.
*/
void *loadSharedLibrary(const char *filename);
/*! Load the function from the library. */
void *getFunction(void *handle, const char *function);
/*! Free the handle to the library. */
int freeSharedLibrary(void * handle);

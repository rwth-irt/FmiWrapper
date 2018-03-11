# FmiWrapper
Simplified interface to call fmi2 models from different languages.

## Usage
The core of this function is located in the "C Code" directory. SystemFunctions.h provides the simplified interface which can be exported to a shared library. Use make with gcc or the VisualStudio solution to build the shared library.

Additionally the VisualStudio solution provides a wrapper for .NET written in C#. By using the simplified interface PInvoke can be used to call into the FmiWrapper.dll which will load the FMU for you. The FmuInstance takes care of the unmanaged resources and ensures that they are freed on dispose or destruction.

## Build notes
- Written in C99
- Comments for doxygen using qt format
- Built successfully with mingw-w64 & VisualStudio on Windows

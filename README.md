# FmiWrapper
Simplified interface to call fmi2 models (see http://fmi-standard.org/) from different languages.

## Usage
The core of this function is located in the [c_wrapper directory](/src/c_wrapper). [fmi_wrapper.h](/src/c_wrapper/fmi_wrapper.h) provides the simplified interface which can be exported to a shared library. The easiest way to build the library is to use cmake.

Additionally the [VisualStudio solution](/src/visual_studio) provides a wrapper for .NET written in C#. By using the simplified interface PInvoke can be used to call into the FmiWrapper.dll which will load the FMU for you. The FmuInstance takes care of the unmanaged resources and ensures that they are freed on dispose or destruction. The FmiWrapper_Net project references the native project and automatically copies the FmiWrapper.dll generated from the build.

The [FmiWrapperConsole](/src/visual_studio/FmiWrapperConsole) is a .net-core console application for testing the capabilities of the wrapper and the fmu. Note that currently the path to the binary is hard-coded to use the win64 version.

## Build notes
- Written in C99
- Comments for doxygen using qt format
- Built using CMake + Visual Studio 2017 & mingw-w64

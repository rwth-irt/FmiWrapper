cmake_minimum_required (VERSION 3.2)
project (fmi_wrapper)
# The version number.
set (fmi_wrapper_VERSION_MAJOR 0)
set (fmi_wrapper_VERSION_MINOR 1)

include_directories("${PROJECT_BINARY_DIR}/c_wrapper")
add_library(fmi_wrapper SHARED fmi_wrapper.c system_functions.c)
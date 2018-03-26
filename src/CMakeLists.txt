cmake_minimum_required (VERSION 3.2)
project (FmiWrapper)
# The version number.
set (FmiWrapper_VERSION_MAJOR 0)
set (FmiWrapper_VERSION_MINOR 1)

 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")
 
# add the executable
add_executable(Tutorial tutorial.cxx)
cmake_minimum_required(VERSION 3.10)

set(librealsenseSDK "/home/pi/librealsense/")

# set the project name
project(adjustHeight)

# add the dlib library
add_subdirectory(/home/pi/dlib-master/dlib/ dlib_build)

# add the executable
add_executable(adjustHeight adjustHeight.cpp)

target_link_libraries(adjustHeight dlib::dlib "/usr/local/lib/librealsense2.so")

target_include_directories(adjustHeight PUBLIC "${PROJECT_BINARY_DIR}" "${librealsenseSDK}/third-party")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
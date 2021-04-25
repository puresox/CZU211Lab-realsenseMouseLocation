# realsenseMouseLocation
深度相机获取嘴部坐标

## 编译

```shell
cd realsenseMouseLocation/realsenseMouseLocation/
mkdir build && cd build
cmake .. && cmake --build . --config Release
cp realsenseMouseLocation ../realsenseMouseLocation
```

## 运行

```shell
cd ~/realsenseMouseLocation/realsenseMouseLocation
./realsenseMouseLocation
```

要在这个目录下执行文件因为要调用`shape_predictor_68_face_landmarks.dat`。也可以在代码中改为绝对路径。

## 环境配置

1. 将压缩文件放入服务器并解压缩`unzip realsenseMouseLocation-master.zip`。
2. 根据[教程](https://github.com/IntelRealSense/librealsense/blob/development/doc/RaspberryPi3.md)安装librealsense。
3. 安装[dlib](http://dlib.net/)：将dlib压缩文件放到服务器解压缩，使用说明见`dlib/examples/CMakeLists.txt`。
4. 安装[bootstrap](https://www.boost.org/)：`sudo apt-get install libboost-all-dev`。
5. 写`CMakeLists.txt`。

```cmake
cmake_minimum_required(VERSION 3.10)

set(librealsenseSDK "/home/pi/librealsense-master")

# set the project name
project(realsenseMouseLocation)

# add the dlib library
add_subdirectory(/home/pi/dlib-master/dlib/ dlib_build)

# add the executable
add_executable(realsenseMouseLocation realsenseMouseLocation.cpp)

target_link_libraries(realsenseMouseLocation dlib::dlib "/usr/local/lib/librealsense2.so")

target_include_directories(realsenseMouseLocation PUBLIC "${PROJECT_BINARY_DIR}" "${librealsenseSDK}/third-party")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
```


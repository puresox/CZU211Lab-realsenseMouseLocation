# realsenseMouseLocation
深度相机获取嘴部坐标

## 编译

```shell
cd realsenseMouseLocation/realsenseMouseLocation/
mkdir build && cd build
cmake ..
cmake --build . --config Release
cp realsenseMouseLocation ../realsenseMouseLocation
```

## 运行

```shell
cd ~/realsenseMouseLocation/realsenseMouseLocation
./realsenseMouseLocation
```

## 环境配置

1. 将压缩文件放入服务器并解压缩`unzip realsenseMouseLocation-master.zip`。
2. 根据[教程](https://github.com/IntelRealSense/librealsense/blob/development/doc/RaspberryPi3.md)安装librealsense。
3. 安装[dlib](http://dlib.net/)：将dlib压缩文件放到服务器解压缩，使用说明见`dlib/examples/CMakeLists.txt`。
4. 安装[bootstrap](https://www.boost.org/)：`sudo apt-get install libboost-all-dev`。
5. 写`CMakeLists.txt`。


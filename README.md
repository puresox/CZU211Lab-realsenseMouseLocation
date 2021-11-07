# CZU211Lab-realsenseMouseLocation
深度相机获取嘴部坐标

## 环境配置

1. 克隆CZU211Lab-realsenseMouseLocation到树莓派。
2. 根据[教程](https://github.com/acrobotic/Ai_Demos_RPi/wiki/Raspberry-Pi-4-and-Intel-RealSense-D435)安装librealsense环境。
3. 将[dlib](http://dlib.net/)压缩文件放到服务器解压缩就行，不需要单独编译，使用说明见`dlib/examples/CMakeLists.txt`。
4. 安装[bootstrap](https://www.boost.org/)：`sudo apt-get install libboost-all-dev`。
5. 修改CZU211Lab-realsenseMouseLocation下的`CMakeLists.txt`。

## 编译

```shell
cd CZU211Lab-realsenseMouseLocation/realsenseMouseLocation/
mkdir build && cd build
cmake .. && cmake --build . --config Release

cd CZU211Lab-realsenseMouseLocation/adjustHeight/
mkdir build && cd build
cmake .. && cmake --build . --config Release
```

## 运行

```shell
cd ~/CZU211Lab-realsenseMouseLocation/realsenseMouseLocation/build
./realsenseMouseLocation

cd ~/CZU211Lab-realsenseMouseLocation/adjustHeight/build
./adjustHeight
```

要在这个目录下执行文件因为要调用`shape_predictor_68_face_landmarks.dat`。也可以在代码中改为绝对路径。
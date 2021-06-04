// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015-2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <librealsense2/rsutil.h>
#include <dlib/image_processing/frontal_face_detector.h> // Dlib
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <boost/uuid/uuid.hpp> // uuid
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream> // Terminal IO
#include <tuple>
#include <array>
#include "rs_frame_image.h"
#include <time.h>

using namespace dlib;
using namespace std;

//image_window win;
// 创建人脸识别器
frontal_face_detector detector = get_frontal_face_detector();

// 创建并初始化人脸特征点识别器
shape_predictor sp;

// 从2D图片中获取嘴部坐标
auto getMouse2DPoint(rs_frame_image<dlib::rgb_pixel, RS2_FORMAT_RGB8> &img)
{
    std::array<std::tuple<int, int>, 21> points{};

    // cout << "正在处理新图像..." << endl;
    // 显示图像
    //win.clear_overlay();
    //win.set_image(img);
    // 人脸识别
    std::vector<rectangle> dets = detector(img);
    // cout << "检测到人脸数：" << dets.size() << endl;
    if (dets.size() == 0)
    {
        return make_tuple(false, points);
    }
    // 从第一个人脸中识别特征点
    full_object_detection shape = sp(img, dets[0]);
    // std::vector<full_object_detection> shapes;
    // cout << "共有特征点数：" << shape.num_parts() << endl;
    // 输出嘴部20个特征点坐标以及中心坐标
    int xCenterSum = 0, yCenterSum = 0;
    for (int shapeNum = 48; shapeNum < 68; shapeNum++)
    {
        // cout << "第" << shapeNum << "个特征点位置：" << shape.part(shapeNum) << endl;
        int partX = shape.part(shapeNum).x();
        int partY = shape.part(shapeNum).y();
        xCenterSum += partX;
        yCenterSum += partY;
        points[shapeNum - 48] = make_tuple(partX, partY);
    }
    int xCenter = xCenterSum / 20, yCenter = yCenterSum / 20;
    points[20] = make_tuple(xCenter, yCenter);
    // cout << "嘴部中心位置：" << xCenter << "," << yCenter << endl;
    // 展示特征点
    // shapes.push_back(shape);
    // win.add_overlay(render_face_detections(shapes));
    return make_tuple(true, points);
}

auto adjustHeight()
{
    rs2::pipeline pipe;
    pipe.start();
    // 去掉开始前几秒的帧
    for (int i = 0; i < 30; i++)
        rs2::frameset frames = pipe.wait_for_frames();
    while (true)
    {
        rs2::frameset frames = pipe.wait_for_frames();
        // 获取彩色帧，彩色图大小：640*480
        rs2::video_frame vf = frames.get_color_frame();
        // 保存彩色图像
        rs_frame_image<dlib::rgb_pixel, RS2_FORMAT_RGB8> image(vf);
        // 从图像中识别嘴的坐标和距离
        auto res = getMouse2DPoint(image);
        auto succ = get<0>(res);
        auto &points = get<1>(res);
        if (!succ)
        {
            // cout << "未检测到人脸" << endl;
            continue;
        }
        auto x = get<0>(points[20]);
        auto y = get<1>(points[20]);
        if (y < 160)
        {
            cout << "up" << endl;
            continue;
        }
        else if (y > 320)
        {
            cout << "down" << endl;
            continue;
        }
        else
        {
            cout << "stop" << endl;
        }
        break;
    }
    pipe.stop();
}

int main(int argc, char *argv[])
{
    deserialize("/home/pi/CZU211Lab-realsenseMouseLocation/shape_predictor_68_face_landmarks.dat") >> sp;

    // 调整高度
    adjustHeight();

    return EXIT_SUCCESS;
}
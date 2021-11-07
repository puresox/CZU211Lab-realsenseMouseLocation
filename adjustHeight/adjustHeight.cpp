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
    rs2::align align_to_color(RS2_STREAM_COLOR);
    // 去掉开始前几秒的帧
    for (int i = 0; i < 30; i++)
        rs2::frameset frames = pipe.wait_for_frames();
    while (true)
    {
        rs2::frameset frames = pipe.wait_for_frames();
        // 替换为对齐后的数据，不对其的话彩色图大小：640*480，深度图大小：848*480
        frames = align_to_color.process(frames);
        // 获取深度帧和彩色帧
        rs2::depth_frame depth = frames.get_depth_frame();
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
        float dist_to_mouse = 0.0;
        for (int i = 20; (i >= 0) && (dist_to_mouse == 0.0); i--)
        {
            auto &point = points[i];
            auto x = get<0>(point);
            auto y = get<1>(point);
            dist_to_mouse = depth.get_distance(x, y);
        }
        auto x = get<0>(points[20]);
        auto y = get<1>(points[20]);
        if (dist_to_mouse == 0.0)
        {
            // cout << "距离为0，重新检测" << endl;
            continue;
        }
        // cout << "距离嘴" << dist_to_mouse << "米远 " << endl;
        // 坐标转换
        float point[3];
        float pixel[2]{x, y};
        rs2_intrinsics intr = frames.get_profile().as<rs2::video_stream_profile>().get_intrinsics();
        rs2_deproject_pixel_to_point(point, &intr, pixel, dist_to_mouse);
        // 输出三维坐标
        // cout << "三维坐标计算完毕：" << point[0] << " , " << point[1] << " , " << point[2] << endl;
        cout << "(" << point[0] << "," << point[1] << "," << point[2] << ")" << endl;
    }
    pipe.stop();
}

int main(int argc, char *argv[])
{
    deserialize("/home/pi/CZU211Lab-realsenseMouseLocation/shape_predictor_68_face_landmarks.dat") >> sp;

    // 获取嘴部三维坐标
    adjustHeight();

    return EXIT_SUCCESS;
}
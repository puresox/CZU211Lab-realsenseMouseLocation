
#include <dlib/image_processing/frontal_face_detector.h> // Dlib
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <opencv2/opencv.hpp> // opencv
#include <boost/filesystem.hpp>
#include <iostream>
#include <chrono>

using namespace dlib;
using namespace std;
using namespace chrono;
using namespace cv;

// 获取路径下的所有图片文件名
void readFilenamesBoost(std::vector<string>& filenames, const string folder)
{
	boost::filesystem::path directory(folder);
	boost::filesystem::directory_iterator itr(directory), end_itr;
	string current_file = itr->path().string();

	for (; itr != end_itr; ++itr)
	{
		if (is_regular_file(itr->path()))
		{
			string filename = itr->path().filename().string(); // 获取图片文件名
			//string filename = itr->path().string(); // 获取图片路径
			filenames.push_back(filename);
		}
	}
}


// 在图像上画直线
void drawLine(cv::Mat& img, int x1, int x2, int y1, int y2) {
	cv::line(img, Point(x1, y1), Point(x2, y2), Scalar(0, 255, 0), 1);
}

// 在图像上画出嘴部
void drawMouth(cv::Mat& img, const full_object_detection& shape) {
	// 49-60 61-68
	for (int shapeNum = 48; shapeNum < 60; shapeNum++)
	{
		int x1 = shape.part((shapeNum - 48) % 12 + 48).x();
		int y1 = shape.part((shapeNum - 48) % 12 + 48).y();
		int x2 = shape.part((shapeNum - 47) % 12 + 48).x();
		int y2 = shape.part((shapeNum - 47) % 12 + 48).y();

		drawLine(img, x1, x2, y1, y2);
	}
	for (int shapeNum = 60; shapeNum < 68; shapeNum++)
	{
		int x1 = shape.part((shapeNum - 60) % 8 + 60).x();
		int y1 = shape.part((shapeNum - 60) % 8 + 60).y();
		int x2 = shape.part((shapeNum - 59) % 8 + 60).x();
		int y2 = shape.part((shapeNum - 59) % 8 + 60).y();

		drawLine(img, x1, x2, y1, y2);
	}
}

int main(int argc, char** argv)
{
	try
	{
		// 初始化
		frontal_face_detector detector = get_frontal_face_detector();
		shape_predictor sp;
		deserialize("./shape_predictor_68_face_landmarks.dat") >> sp;

		// 获取测试集所有图片
		std::vector<string> filenames = {};
		readFilenamesBoost(filenames, "./faces");
		auto subStart = filenames.begin();
		auto subEnd = filenames.begin() + 100;
		std::vector<string> subVector(subStart, subEnd);
		int filenum = subVector.size();
		int correctnum = 0;

		// 循环处理每一张图片
		for (const string& filename : subVector)
		{
			cout << "processing image " << filename << endl;
			array2d<unsigned char> dlibImg;
			load_image(dlibImg, "./faces/" + filename);
			while (dlibImg.nc() < 80 || dlibImg.nr() < 80)
			{
				//pyramid_up(dlibImg);
			}

			// 识别人脸
			auto start = system_clock::now();
			std::vector<dlib::rectangle> dets = detector(dlibImg);
			auto end = system_clock::now();
			auto duration = duration_cast<microseconds>(end - start);
			cout << "人脸识别花费了"
				<< double(duration.count()) * microseconds::period::num / microseconds::period::den
				<< "秒" << endl;

			// 没识别出来的继续下一轮
			cout << "Number of faces detected: " << dets.size() << endl;
			if (dets.size() == 0) continue;

			// 识别人脸特征
			full_object_detection shape = sp(dlibImg, dets[0]);
			cout << "共有特征点数：" << shape.num_parts() << endl;

			// 画出人脸特征
			cv::Mat opencvImg;
			opencvImg = cv::imread("./faces/" + filename);

			drawMouth(opencvImg, shape);
			cv::imwrite("./faces_render/" + filename, opencvImg);

			correctnum++;
		}
		float rate = static_cast<float>(correctnum) / static_cast<float>(filenum);
		cout << "准确率：" << rate * 100 << "%" << endl;
	}
	catch (exception& e)
	{
		cout << "\nexception thrown!" << endl;
		cout << e.what() << endl;
	}
}

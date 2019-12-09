#pragma once
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include<opencv2/opencv.hpp> 
#include <opencv2/core.hpp>

//typedef struct Rect_struct {
//	int tl_x;
//	int tl_y;
//	int br_x;
//	int br_y;
//} Rect_s;
class Rect_s {
public:
		Rect_s(int tl_x_temp, int tl_y_temp, int br_x_temp, int br_y_temp) {
			int tl_x = tl_x_temp;
			int tl_y = tl_y_temp;
			int br_x = br_x_temp;
			int br_y = br_y_temp;
		}//构造函数
		Rect_s() {
			tl_x=0;
			tl_y=0;
			br_x=0;
			br_y=0;

		}
		Rect_s(const Rect_s &rect) {
			tl_x = rect.tl_x;
			tl_y = rect.tl_y;
			br_x = rect.br_x;
			br_y = rect.br_y;
		}//复制构造函数

		int get_width() { return (br_x - tl_x); }
		int get_height() { return (br_y - tl_y); }
public:
		int tl_x;
		int tl_y;
		int br_x;
		int br_y;
		int width;
		int height;
};
typedef struct car {
	float point_x;
	float point_y;
	float width_x;
	float height_x;

} car_data;
cv::Mat kernel_circle(int size1, int size2);
float getMSSIM(cv::Mat  inputimage1, cv::Mat inputimage2);
Rect_s get_car_points(cv::Mat img_0, cv::Mat img_1);
Rect_s get_box_point(cv::Mat img_0, cv::Mat img_1);
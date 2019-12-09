#include "stdafx.h"
#include <stdio.h>
#include <iostream>  
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include<opencv2/opencv.hpp> 
#include <opencv2/core.hpp>
#include "auto.h"
using namespace cv;
using namespace std;
Mat kernel_circle(int size1, int size2)
{
	Mat kernel_1 = Mat::zeros(Size(size1, size1), CV_8UC3);
	Mat kernel_2 = Mat::zeros(Size(size1, size1), CV_8UC3);

	if (size1 % 2 == 0 || size2 % 2 == 0)
	{
		printf("ciRcle size must be odd");
	}
	int centre_x = int((size1 - 1) / 2);
	int centre_y = int((size1 - 1) / 2);
	int r1 = (size1 - 1) / 2;
	int r2 = (size2 - 1) / 2;
	circle(kernel_1, Point(centre_x, centre_y), r1, Scalar(255, 255, 255), -1);
	circle(kernel_2, Point(centre_x, centre_y), r2, Scalar(255, 255, 255), -1);
	Mat kernel = kernel_1 - kernel_2;
	kernel.convertTo(kernel, CV_8UC3);
	Mat dst;
	cvtColor(kernel, dst, COLOR_BGR2GRAY);
	return dst;
}
float getMSSIM(Mat  inputimage1, Mat inputimage2)
{
	Mat i1 = inputimage1;
	Mat i2 = inputimage2;
	const double C1 = 6.5025, C2 = 58.5225;
	int d = CV_32F;
	Mat I1, I2;
	i1.convertTo(I1, d);
	i2.convertTo(I2, d);
	Mat I2_2 = I2.mul(I2);
	Mat I1_2 = I1.mul(I1);
	Mat I1_I2 = I1.mul(I2);
	Mat mu1, mu2;
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);
	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);
	Mat sigma1_2, sigma2_2, sigma12;
	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;
	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;
	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;
	Mat t1, t2, t3;
	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);
	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);
	Mat ssim_map;
	divide(t3, t1, ssim_map);
	Scalar mssim = mean(ssim_map);
	float res = abs((mssim.val[0] + mssim.val[1] + mssim.val[2]) / 3);
	return res;
}
Rect_s get_car_points(Mat img_0, Mat img_1) //通过帧差法及相似度去除干扰求得车体
{
	//Mat p0_re;
	//Mat p1_re;
	//resize(img_0, p0_re, Size(), 0.5, 0.5);//更改尺寸，方便快速计算，这个可以 参数可以调整， 800是列， 400是行
	//resize(img_1, p1_re, Size(), 0.5, 0.5);
	//获得图片的大小
	//Size pic_size = p0_re.size();  //size类是先存宽(columns)，后存高(rows) cols=1280 rows=680
	//int  cols = p0_re.cols;
	//int rows = p0_re.rows;
	//int channels = p0_re.channels();
	int  cols = img_0.cols;
	int rows = img_0.rows;
	int channels = img_0.channels();
	//printf("cols%d,rows:%d,channels:%d\n", cols, rows, channels);
	//归一化（归一化的目的是为了将所有像素点的范围从0-255转变到0-1，在求差值时，方便了解图片像素点的相对变化程度
	Mat p0_to_one;
	Mat p1_to_one;
	img_0.convertTo(p0_to_one, CV_32FC3, 1 / 255.0);//转变数据类型到浮点型，否则会判别时，差值的结果只有0和1
	img_1.convertTo(p1_to_one, CV_32FC3, 1 / 255.0);
	//p3存储判断结果
	Mat result(rows, cols, CV_8UC1);
	//计算两张图片的差值
	Mat chazhi(rows, cols, CV_32FC3, Scalar_<float>(123, 34325));
	absdiff(p1_to_one, p0_to_one, chazhi);//求两张归一化后图片的差值
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if ((chazhi.at<Vec3f>(i, j)[0] < 0.15) && (chazhi.at<Vec3f>(i, j)[1] < 0.15) && (chazhi.at<Vec3f>(i, j)[2] < 0.15))//设定的三通道阈值都为0.15
			{
				result.at<uchar>(i, j) = 0;
			}
			else
			{
				result.at<uchar>(i, j) = 1;
			}
		}
	}
	//Mat element_o = getStructuringElement(MORPH_RECT, Size(5, 5));//这是进行为后面的开运算准备一个 5x5的方形核
	//Mat element_c = getStructuringElement(MORPH_RECT, Size(3, 3));//这是进行为后面的闭运算准备一个 3x3的方形核
	//Mat open_pic, close_pic;
	//morphologyEx(result, open_pic, MORPH_OPEN, element_o);   //开运算可以去除噪点
	//morphologyEx(open_pic, close_pic, MORPH_CLOSE, element_c);   //开运算可以去除噪点

	//imshow("close", close_pic);
	//waitKey(0);
	//morphologyEx(result, close_pic, MORPH_CLOSE, element);//闭运算可以填补缝隙
	Mat result_double;
	result.convertTo(result_double, CV_64F);
	Mat kernel = kernel_circle(31, 29);
	Mat goal = Mat::zeros(Size(cols, rows), CV_64F);
	filter2D(result_double, goal, CV_64F, kernel);
	double minVal = 0.01, maxVal = 0.01;
	int* minIdx = 0;
	int* maxIdx = 0;
	minMaxIdx(goal, &minVal, &maxVal, minIdx, maxIdx);

	double thre_g = maxVal * 0.3;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (goal.at<double>(i, j) < thre_g)
				result.at<uchar>(i, j) = 0;
			else
				result.at<uchar>(i, j) = result.at<uchar>(i, j) * 255;
		}

	}

	vector<vector<Point>> contours;
	findContours(result, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);//寻找预处理后图片的轮廓，contours中会有所有的轮廓，后面需要通过以寻找面积最大的方式提取出来
	vector<Point> max_contours;  //用于存储最大轮廓（轮廓是由一系列点边缘点组成）
	double maxArea = 0;
	//下面是寻找面积最大轮廓
	for (size_t i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		if (area > maxArea)
		{
			maxArea = area;
			max_contours = contours[i];
		}
	}
	//RotatedRect minArea_rect = minAreaRect(max_contours);//用一个旋转的矩形去拟合得到的最大的轮廓
	//angle_ = minArea_rect.angle;
	////Point2f pts[4];//抠图方案得到的旋转矩形的顶点坐标存在pts中
	////minArea_rect.points(pts);
	//Mat pts;
	//boxPoints(minArea_rect, pts);
	//return pts;

//9.17将minArea 修改为boundingRect
	Rect bdrect = boundingRect(max_contours);
	Rect_s res(0,0,0,0);
	res.tl_x = bdrect.x;
	res.tl_y = bdrect.y;
	res.br_x = bdrect.width + bdrect.x;
	res.br_y = bdrect.height + bdrect.y;
	//误差设为0.05 车宽比设为
	float error_rate = 0.05;
	float bz = 2.777778;
	float thre_min = bz * (1 - error_rate);
	float thre_max = bz * (1 + error_rate);

	int tl_x = res.tl_x;
	int tl_y = res.tl_y;
	int br_x = res.br_x;
	int br_y = res.br_y;
	float  rect_width = br_x - tl_x;
	float rect_heigth = br_y - tl_y;
	float rect_rate = (rect_heigth / rect_width);
	if (rect_rate > thre_min && rect_rate < thre_max)
	{
		return res;

	}
	if (rect_rate < thre_min) //说明宽方向有阴影
	{
		int compare_width = int(abs(rect_heigth / bz - rect_width));
		Mat region_width_left = img_1(Rect(Point(tl_x, tl_y), Point((tl_x + compare_width), br_y)));
		Mat region_width_right = img_1(Rect(Point((br_x - compare_width), tl_y), Point(br_x, br_y)));
		//imshow("111", region_width_left);
		//imshow("222", region_width_right);
		//waitKey(0);
		Mat origion_width_left = img_0(Rect(Point(tl_x, tl_y), Point((tl_x + compare_width), br_y)));
		Mat origion_width_right = img_0(Rect(Point((br_x - compare_width), tl_y), Point(br_x, br_y)));
		float res_left = getMSSIM(origion_width_left, region_width_left);
		float res_right = getMSSIM(origion_width_right, region_width_right);
		//printf(".3f,.3f", res_left, res_right);
		if (res_left >= res_right)
		{
			res.tl_x = (tl_x + compare_width);
			return res;
		}
		else
		{
			res.br_x = (br_x - compare_width);
			return res;
		}

	}
	if (rect_rate >= thre_max)
	{
		int compare_height = int(abs(rect_width*bz - rect_heigth));
		Mat region_height_top = img_1(Rect(Point(tl_x, tl_y), Point(br_x, (tl_y + compare_height))));
		Mat region_height_bottom = img_1(Rect(Point(tl_x, (br_y - compare_height)), Point(br_x, br_y)));

		Mat origion_height_top = img_0(Rect(Point(tl_x, tl_y), Point(br_x, (tl_y + compare_height))));
		Mat origion_height_bottom = img_0(Rect(Point(tl_x, (br_y - compare_height)), Point(br_x, br_y)));

		float res_top = getMSSIM(origion_height_top, region_height_top);
		float res_bottom = getMSSIM(origion_height_bottom, region_height_bottom);
		if (res_bottom >= res_top)
		{
			res.br_y = br_y - compare_height;
			return res;
		}
		else
		{
			res.tl_y = tl_y + compare_height;
			return res;
		}
	}

}
Rect_s get_box_point(Mat img_0,Mat img_1) //通过数据库数据得知
{
	//cvui::init(WINDOW_NAME);
	Mat img_reset = img_1.clone();
	Rect_s rect_element(0,0,0,0);
	rect_element = get_car_points(img_0, img_1);
	printf("%d,%d,%d,%d", rect_element.tl_x, rect_element.tl_y, rect_element.br_x, rect_element.br_y);
	//通过抠图的方法得到x0-x4 ，y0-y4的四个初始值作为参考区域

	int x0_c = rect_element.tl_x;
	int y0_c = rect_element.tl_y;
	int x1_c = rect_element.tl_x;
	int y1_c = rect_element.br_y;
	int x2_c = rect_element.br_x;
	int y2_c = rect_element.br_y;
	int x3_c = rect_element.br_x;
	int y3_c = rect_element.tl_y;
	//rectangle(img_1, Point(x0_c, y0_c), Point(x2_c, y2_c),Scalar(255,0,0),1);
	//imshow("zxs", img_1);
	//waitKey(0);
	car_data car1;
	car1.point_x = 0.12;
	car1.point_y = 0.04;
	car1.width_x = 0.57;
	car1.height_x = 0.58;
	Rect_s box(0,0,0,0);
	//int box_x0 = int(x0_c + car1.point_x*(x2_c - x0_c));
	int box_x0 = int((x2_c + x0_c) / 2 - (x2_c - x0_c)*car1.width_x / 2);
	int box_y0 = int(y0_c + car1.point_y*(y2_c - y0_c));
	//int box_x2 = int(box_x0+car1.width_x*(x2_c - x0_c));
	int box_x2 = int((x2_c + x0_c) / 2 + (x2_c - x0_c)*car1.width_x / 2);
	int box_y2 = int(box_y0 + car1.height_x*(y2_c - y0_c));

	box.tl_x = box_x0;
	box.tl_y = box_y0;
	box.br_x = box_x2;
	box.br_y = box_y2;
	//根据抠图结果及数据库信息预测车厢所在位置;
	//rectangle(img_1, Point(x0_c,y0_c), Point(x2_c,y2_c), Scalar(0, 0, 255), 2);//抠图目标所在区域
	//rectangle(img_1, Point(box_x0, box_y0), Point(box_x2, box_y2), Scalar(255, 255, 0), 1);
	//putText(img_1, format("x0: %d,y0: %d", box_x0, box_y0), Point(10, 100), 0, 0.5, Scalar(5, 50, 255));
	//putText(img_1, format("x1: %d,y1: %d", box_x0, box_y2), Point(10, 140), 0, 0.5, Scalar(5, 50, 255));
	//putText(img_1, format("x2: %d,y2: %d", box_x2, box_y2), Point(10, 180), 0, 0.5, Scalar(5, 50, 255));
	//putText(img_1, format("x3: %d,y3: %d", box_x2, box_y0), Point(10, 220), 0, 0.5, Scalar(5, 50, 255));

	//system("pause");
	return box;
}

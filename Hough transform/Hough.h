#pragma once
#include "CImg.h"
#include <string>
#include <vector>
#include <iostream>
#include <time.h>

using namespace cimg_library;
using namespace std;
typedef unsigned char uchar;

#define M_PI acos(-1.0)

struct Point {
	int x, y;
	Point(int a, int b) {
		x = a;
		y = b;
	}
};

struct Line {
	double k, b;
	Line(double x, double y) {
		k = x;
		b = y;
	}
};

class Hough {
private:
	CImg<uchar> img; //Original Image
	CImg<uchar> gFiltered;
	CImg<int> houghImage;  //Hough Space
	CImg<float> result;
	vector<Point> peaks;
	vector<Line> lines;
	vector<Point> points;	//交点
	CImg<double> gradnum;
	vector<Point> circles;
	vector<Point> center;
	vector<int> circleWeight;
	int width, height, max_length;


	int theta = 360;
	double interval = M_PI / 180;
	int min_votes = 200;	//最小投票数
	int min_distance = 200;		//最小距离
	int gradLimit = 4;		//梯度阀值

	//圆
	double minRadius = 200;
	int minR = 150, maxR = 300;
	int rLimit = 80;		//筛选半径
	int voteLimit = 40;		//筛选圆心

public:
	Hough(string);
	void sobel();	//Sobel filtering
	void Prewitt();
	void houghSpaceTransform();	//霍夫空间变换
	void houghLinesDetect();		//霍夫直线检测
	double distance(double, double);
	void drawLines();		//寻找并画出直线
	void drawPoints();		//寻找并画出焦点
	void houghCircleTransform();	//霍夫圆变换
	void houghCirclesDetect();
	void drawCircle(int r);
};
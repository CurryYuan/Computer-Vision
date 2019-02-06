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
	CImg<uchar> gradnum;
	vector<Point> circles;
	vector<Point> center;
	vector<int> circleWeight;
	int width, height, max_length;


	int theta = 180;
	double interval = M_PI / 180;
	int min_votes = 150;	//最小投票数
	int min_distance = 100;		//最小距离

public:
	vector<Point> points;	//交点

	Hough(string);
	void houghSpaceTransform();	//霍夫空间变换
	void houghLinesDetect();		//霍夫直线检测
	double distance(double, double);
	void drawLines();		//寻找并画出直线
	void drawPoints();		//寻找并画出焦点
};
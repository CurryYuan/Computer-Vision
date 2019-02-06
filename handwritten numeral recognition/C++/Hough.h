#pragma once
#include "CImg.h"
#include <string>
#include <vector>
#include <iostream>
#include <time.h>
#include <fstream>
#include <algorithm>

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

struct votePoint {
	Point p;
	int vote;
	votePoint(Point a, int b):p(a),vote(b) {

	}
};

class Hough {
private:
	CImg<uchar> img; //Original Image
	CImg<uchar> gFiltered;
	CImg<int> houghImage;  //Hough Space
	CImg<uchar> resultImg;
	vector<votePoint> peaks;
	//直线点集
	vector< pair< pair<int, int>, pair<int, int> > > lines;
	//四个角点
	CImg<uchar> gradnum;
	vector<Point> circles;
	vector<Point> center;
	vector<int> circleWeight;
	string fileName;

	int theta = 180;
	double interval = M_PI / 180;
	int min_votes = 300;	//最小投票数
	int min_distance = 500;		//最小距离
	int DIFF = 10;
	int gradLimit = 6;
	int Red[3] = { 255, 0, 0 };

public:
	int width, height, max_length;
	vector<Point> points;	//交点
	vector< pair<int, int > > vertex;


	Hough(string);
	void Prewitt();
	void houghSpaceTransform();	//霍夫空间变换
	double distance(double, double);
	void drawLines();		//寻找并画出直线
	void drawPoints();		//寻找并画出焦点
	void orderVertexs();	//排序
};
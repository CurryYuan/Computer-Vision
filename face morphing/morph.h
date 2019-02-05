#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

struct point {
	int x;
	int y;
	int index;	//保存点在vector中的索引
	point(int _x, int _y, int _index=-1) : x(_x), y(_y), index(_index) {}
};

inline bool operator == (const point& a, const point& b) {
	return (a.x == b.x) && (a.y == b.y);
}

inline ostream &operator << (ostream &out, point const &point) {
	return out << "Point x: " << point.x << " y: " << point.y;
}


class Edge {
public:
	Edge(const point &a, const point &b) : a(a), b(b) {};
	Edge(const Edge &e) : a(e.a), b(e.b) {};

	point a;
	point b;
};

inline ostream &operator << (ostream &out, Edge const &e) {
	return out << "Edge " << e.a << ", " << e.b;
}

inline bool operator == (const Edge& e1, const Edge& e2) {
	return 	(e1.a == e2.a && e1.b == e2.b) ||
		(e1.a == e2.b && e1.b == e2.a);
}


struct triangle {
	point a;
	point b;
	point c;
	int index[3];
	Edge e1, e2, e3;
	triangle(point _a, point _b, point _c, int a_index, int b_index, int c_index)
		: a(_a), b(_b), c(_c), e1(_a,_b), e2(_b,_c), e3(_c,_a) {
		index[0] = a_index;
		index[1] = b_index;
		index[2] = c_index;
	}
	triangle(point _a, point _b, point _c) : a(_a), b(_b), c(_c), e1(_a, _b), e2(_b, _c), e3(_c, _a) {}

	bool isInTriangle(const point& p);
	bool containsVertex(const point &p);  // 判断点p是否是三角形的顶点
};


inline bool operator == (const triangle &t1, const triangle &t2) {
	return	(t1.a == t2.a || t1.a == t2.b || t1.a == t2.c) &&
		(t1.b == t2.a || t1.b == t2.b || t1.b == t2.c) &&
		(t1.c == t2.a || t1.c == t2.b || t1.c == t2.c);
}


class morph
{
public:
	morph();
	CImg<float> getDetectionPoints(vector<point*>& points, const CImg<float>& src, string filename);
	double getDistance(point A, point B);
	bool isPointInCircle(point* P, triangle* tri);
	void getDelaunayTriangles(vector<triangle*>& triangles, const vector<point*>& points);
	void drawTriangles(const vector<triangle*>& triangles, const CImg<float>& src);
	triangle* getTransitionTriangle(const triangle* A, const triangle* B, double rate);
	CImg<float> getTransTriangle2Triangle( const triangle* src, const triangle* dst);
	CImg<float> morphTriangle(double rate, const vector<triangle*>& triangles_A, const vector<triangle*>& triangles_B,
		const CImg<float>& img_A, const CImg<float>& img_B);
};

int cross3(const point &a, const point &b, const point &p);


#pragma once
#include "CImg.h"
#include <string>
#include <vector>
#include <iostream>
#include <time.h>

using namespace cimg_library;
using namespace std;
typedef unsigned char uchar;

struct Point {
	int x, y;
	Point(int a, int b) {
		x = a;
		y = b;
	}
};

class canny
{
private:
	CImg<uchar> img; //Original Image
	CImg<uchar> grayscaled; // Grayscale
	CImg<uchar> gFiltered; // Gradient
	CImg<uchar> sFiltered; //Sobel Filtered
	CImg<float> angles; //Angle Map
	CImg<uchar> non; // Non-maxima supp.
	CImg<uchar> thres; //Double threshold and final
	CImg<uchar> edge;
public:
	canny(string); //Constructor
	CImg<uchar> toGrayScale();
	vector<vector<double>> createFilter(int, int, double); //Creates a gaussian filter
	CImg<uchar> useFilter(CImg<uchar>, vector<vector<double>>); //Use some filter
	CImg<uchar> sobel(); //Sobel filtering
	CImg<uchar> nonMaxSupp(); //Non-maxima supp.
	CImg<uchar> threshold(CImg<uchar>, int, int); //Double threshold and finalize picture
	CImg<uchar> edgeTrack(CImg<uchar>);
};

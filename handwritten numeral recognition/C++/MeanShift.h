#pragma once
#include "CImg.h"
#include <iostream>
#include <vector>

using namespace cimg_library;
using namespace std;

typedef unsigned char uchar;

// 5-Dimensional Point
class Point5D {
public:
	float x;			// Spatial value
	float y;			// Spatial value
	float l;			// Lab value
	float a;			// Lab value
	float b;			// Lab value
public:
	Point5D();													// Constructor
	Point5D(float, float, float, float, float);					// Set point value
	void PointLab();											// Scale the Lab color to Lab range
	void PointRGB();											// Sclae the Lab color to range that can be used to transform to RGB
	void MSPoint5DAccum(Point5D);								// Accumulate points
	float MSPoint5DColorDistance(Point5D);						// Compute color space distance between two points
	float MSPoint5DSpatialDistance(Point5D);					// Compute spatial space distance between two points
	void MSPoint5DScale(float);									// Scale point
	
	void Print();												// Print 5D point
};

class MeanShift
{
public:
	MeanShift(float ,float);		// Constructor for spatial bandwidth and color bandwidth
	void MSSegmentation(CImg<>&);									// Mean Shift Segmentation

	float hs;			//spatial radius
	float hr;			//color radius
};


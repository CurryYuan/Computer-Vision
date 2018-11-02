//
//  canny.h
//  Canny Edge Detector
//
//  Created by Hasan Akgün on 21/03/14.
//  Copyright (c) 2014 Hasan Akgün. All rights reserved.
//

#ifndef _CANNY_
#define _CANNY_
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <time.h>

using namespace std;
using namespace cv;

class canny {
private:
    Mat img; //Original Image
    Mat grayscaled; // Grayscale
    Mat gFiltered; // Gradient
    Mat sFiltered; //Sobel Filtered
    Mat angles; //Angle Map
    Mat non; // Non-maxima supp.
    Mat thres; //Double threshold and final
	Mat edge;	//edge trace
public:

    canny(String); //Constructor
	Mat toGrayScale();
	vector<vector<double>> createFilter(int, int, double); //Creates a gaussian filter
	Mat useFilter(Mat, vector<vector<double>>); //Use some filter
    Mat sobel(); //Sobel filtering
    Mat nonMaxSupp(); //Non-maxima supp.
    Mat threshold(Mat, int, int); //Double threshold and finalize picture
	Mat edgeTrack(Mat);	//edge trace
};

#endif

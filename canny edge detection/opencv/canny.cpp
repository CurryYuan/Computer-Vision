//
//  canny.cpp
//  Canny Edge Detector
//
//  Created by Hasan Akgün on 21/03/14.
//  Copyright (c) 2014 Hasan Akgün. All rights reserved.
//

#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include "canny.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

#define M_PI 3.14159265359


canny::canny(String filename)
{
	img = imread(filename);
	
	if (!img.data) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;

	}
	else
	{

		vector<vector<double>> filter = createFilter(3, 3, 1);

		//Print filter
		for (int i = 0; i < filter.size(); i++)
		{
			for (int j = 0; j < filter[i].size(); j++)
			{
				cout << filter[i][j] << " ";
			}
		}
		grayscaled = toGrayScale(); //Grayscale the image
		gFiltered = Mat(useFilter(grayscaled, filter)); //Gaussian Filter
		sFiltered = Mat(sobel()); //Sobel Filter

		non = Mat(nonMaxSupp()); //Non-Maxima Suppression
		thres = Mat(threshold(non, 40, 100)); //Double Threshold and Finalize
		edge = Mat(edgeTrack(thres.clone()));

		/*namedWindow("Original");
		namedWindow("GrayScaled");
		namedWindow("Gaussian Blur");
		namedWindow("Sobel Filtered");
		namedWindow("Non-Maxima Supp.");
		namedWindow("Final");*/

		imwrite("./result/Original.bmp", img);
		imwrite("./result/GrayScaled.bmp", grayscaled);
		imwrite("./result/Gaussian Blur.bmp", gFiltered);
		imwrite("./result/Sobel Filtered.bmp", sFiltered);
		imwrite("./result/Non-Maxima Supp.bmp", non);
		imwrite("./result/Final.bmp", thres);
		imwrite("./result/edge.bmp", edge);
		waitKey(0);
	}
}

Mat canny::toGrayScale()
{
    grayscaled = Mat(img.rows, img.cols, CV_8UC1); //To one channel
	for (int i = 0; i < img.rows; i++)
		for (int j = 0; j < img.cols; j++)
		{
			int b = img.at<Vec3b>(i, j)[0];
			int g = img.at<Vec3b>(i, j)[1];
			int r = img.at<Vec3b>(i, j)[2];

			double newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
			grayscaled.at<uchar>(i, j) = newValue;

		}
    return grayscaled;
}

vector<vector<double>> canny::createFilter(int row, int column, double sigmaIn)
{
	vector<vector<double>> filter;

	for (int i = 0; i < row; i++)
	{
        vector<double> col;
        for (int j = 0; j < column; j++)
        {
            col.push_back(-1);
        }
		filter.push_back(col);
	}

	float coordSum = 0;
	float constant = 2.0 * sigmaIn * sigmaIn;

	// Sum is for normalization
	float sum = 0.0;

	for (int x = - row/2; x <= row/2; x++)
	{
		for (int y = -column/2; y <= column/2; y++)
		{
			coordSum = (x*x + y*y);
			filter[x + row/2][y + column/2] = (exp(-(coordSum) / constant)) / (M_PI * constant);
			sum += filter[x + row/2][y + column/2];
		}
	}

	// Normalize the Filter
	for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            filter[i][j] /= sum;

	return filter;

}

Mat canny::useFilter(Mat img_in, vector<vector<double>> filterIn)
{
    int size = (int)filterIn.size()/2;
	Mat filteredImg = Mat(img_in.rows - 2*size, img_in.cols - 2*size, CV_8UC1);
	for (int i = size; i < img_in.rows - size; i++)
	{
		for (int j = size; j < img_in.cols - size; j++)
		{
			double sum = 0;
            
			for (int x = 0; x < filterIn.size(); x++)
				for (int y = 0; y < filterIn.size(); y++)
				{
                    sum += filterIn[x][y] * (double)(img_in.at<uchar>(i + x - size, j + y - size));
				}
            
            filteredImg.at<uchar>(i-size, j-size) = sum;
		}

	}
	return filteredImg;
}

Mat canny::sobel()
{

    //Sobel X Filter
    double x1[] = {-1.0, 0, 1.0};
    double x2[] = {-2.0, 0, 2.0};
    double x3[] = {-1.0, 0, 1.0};

    vector<vector<double>> xFilter(3);
    xFilter[0].assign(x1, x1+3);
    xFilter[1].assign(x2, x2+3);
    xFilter[2].assign(x3, x3+3);
    
    //Sobel Y Filter
    double y1[] = {1.0, 2.0, 1.0};
    double y2[] = {0, 0, 0};
    double y3[] = {-1.0, -2.0, -1.0};
    
    vector<vector<double>> yFilter(3);
    yFilter[0].assign(y1, y1+3);
    yFilter[1].assign(y2, y2+3);
    yFilter[2].assign(y3, y3+3);
    
    //Limit Size
    int size = (int)xFilter.size()/2;
    
	Mat filteredImg = Mat(gFiltered.rows - 2*size, gFiltered.cols - 2*size, CV_8UC1);
    
    angles = Mat(gFiltered.rows - 2*size, gFiltered.cols - 2*size, CV_32FC1); //AngleMap

	for (int i = size; i < gFiltered.rows - size; i++)
	{
		for (int j = size; j < gFiltered.cols - size; j++)
		{
			double sumx = 0;
            double sumy = 0;
            
			for (int x = 0; x < xFilter.size(); x++)
				for (int y = 0; y < yFilter.size(); y++)
				{
                    sumx += xFilter[x][y] * (double)(gFiltered.at<uchar>(i + x - size, j + y - size)); //Sobel_X Filter Value
                    sumy += yFilter[x][y] * (double)(gFiltered.at<uchar>(i + x - size, j + y - size)); //Sobel_Y Filter Value
				}
			double sumxsq = sumx * sumx;
			double sumysq = sumy * sumy;
            
            double sq2 = sqrt(sumxsq + sumysq);
            
            if(sq2 > 255) //Unsigned Char Fix
                sq2 =255;
            filteredImg.at<uchar>(i-size, j-size) = sq2;
 
			if (sumx == 0) //Arctan Fix
				angles.at<float>(i - size, j - size) = 90;
			else
				angles.at<float>(i - size, j - size) = atan(sumy / sumx)*(180.0 / M_PI);
		}
	}
    return filteredImg;
}


Mat canny::nonMaxSupp()
{
    Mat nonMaxSupped = Mat(sFiltered.rows-2, sFiltered.cols-2, CV_8UC1);
    for (int i=1; i< sFiltered.rows - 1; i++) {
        for (int j=1; j<sFiltered.cols - 1; j++) {
            float Tangent = angles.at<float>(i,j);
			//cout << Tangent << " ";

            nonMaxSupped.at<uchar>(i-1, j-1) = sFiltered.at<uchar>(i,j);
            //Horizontal Edge
            if (((-22.5 < Tangent) && (Tangent <= 22.5)) || ((157.5 < Tangent) && (Tangent <= -157.5)))
            {
                if ((sFiltered.at<uchar>(i,j) < sFiltered.at<uchar>(i,j+1)) || (sFiltered.at<uchar>(i,j) < sFiltered.at<uchar>(i,j-1)))
                    nonMaxSupped.at<uchar>(i-1, j-1) = 0;
            }
            //Vertical Edge
            if (((-112.5 < Tangent) && (Tangent <= -67.5)) || ((67.5 < Tangent) && (Tangent <= 112.5)))
            {
                if ((sFiltered.at<uchar>(i,j) < sFiltered.at<uchar>(i+1,j)) || (sFiltered.at<uchar>(i,j) < sFiltered.at<uchar>(i-1,j)))
                    nonMaxSupped.at<uchar>(i-1, j-1) = 0;
            }
            
            //-45 Degree Edge
            if (((-67.5 < Tangent) && (Tangent <= -22.5)) || ((112.5 < Tangent) && (Tangent <= 157.5)))
            {
				if ((sFiltered.at<uchar>(i, j) < sFiltered.at<uchar>(i + 1, j + 1)) || (sFiltered.at<uchar>(i, j) < sFiltered.at<uchar>(i - 1, j - 1)))
                //if ((sFiltered.at<uchar>(i,j) < sFiltered.at<uchar>(i-1,j+1)) || (sFiltered.at<uchar>(i,j) < sFiltered.at<uchar>(i+1,j-1)))
                    nonMaxSupped.at<uchar>(i-1, j-1) = 0;
            }
            
            //45 Degree Edge
            if (((-157.5 < Tangent) && (Tangent <= -112.5)) || ((22.5 < Tangent) && (Tangent <= 67.5)))
            {
				if ((sFiltered.at<uchar>(i, j) < sFiltered.at<uchar>(i - 1, j + 1)) || (sFiltered.at<uchar>(i, j) < sFiltered.at<uchar>(i + 1, j - 1)))
                //if ((sFiltered.at<uchar>(i,j) < sFiltered.at<uchar>(i+1,j+1)) || (sFiltered.at<uchar>(i,j) < sFiltered.at<uchar>(i-1,j-1)))
                    nonMaxSupped.at<uchar>(i-1, j-1) = 0;
            }
        }
    }
    return nonMaxSupped;
}

Mat canny::threshold(Mat imgin,int low, int high)
{
    if(low > 255)
        low = 255;
    if(high > 255)
        high = 255;
    
    Mat EdgeMat = Mat(imgin.rows, imgin.cols, imgin.type());
    
    for (int i=0; i<imgin.rows; i++) 
    {
        for (int j = 0; j<imgin.cols; j++) 
        {
            EdgeMat.at<uchar>(i,j) = imgin.at<uchar>(i,j);
            if(EdgeMat.at<uchar>(i,j) > high)
                EdgeMat.at<uchar>(i,j) = 255;
            else if(EdgeMat.at<uchar>(i,j) < low)
                EdgeMat.at<uchar>(i,j) = 0;
            else
            {
                bool anyHigh = false;
                bool anyBetween = false;
                for (int x=i-1; x < i+2; x++) 
                {
                    for (int y = j-1; y<j+2; y++) 
                    {
                        if(x <= 0 || y <= 0 || x >= EdgeMat.rows || y >= EdgeMat.cols) //Out of bounds
                            continue;
                        else
                        {
                            if(EdgeMat.at<uchar>(x,y) > high)
                            {
                                EdgeMat.at<uchar>(i,j) = 255;
                                anyHigh = true;
                                break;
                            }
                            else if(EdgeMat.at<uchar>(x,y) <= high && EdgeMat.at<uchar>(x,y) >= low)
                                anyBetween = true;
                        }
                    }
                    if(anyHigh)
                        break;
                }
                if(!anyHigh && anyBetween)
                    for (int x=i-2; x < i+3; x++) 
                    {
                        for (int y = j-1; y<j+3; y++) 
                        {
                            if(x < 0 || y < 0 || x >= EdgeMat.rows || y >= EdgeMat.cols) //Out of bounds
                                continue;
                            else
                            {
                                if(EdgeMat.at<uchar>(x,y) > high)
                                {
                                    EdgeMat.at<uchar>(i,j) = 255;
                                    anyHigh = true;
                                    break;
                                }
                            }
                        }
                        if(anyHigh)
                            break;
                    }
                if(!anyHigh)
                    EdgeMat.at<uchar>(i,j) = 0;
            }
        }
    }
    return EdgeMat;
}

Mat canny::edgeTrack(Mat Edge)
{

	// 8 neighbors
	const Point directions[8] = { { 0, 1 }, {1,1}, { 1, 0 }, { 1, -1 }, { 0, -1 },  { -1, -1 }, { -1, 0 },{ -1, 1 } };

	vector<Point> edge_t;
	vector<vector<Point>> edges;

	// 边缘跟踪
	int i, j, counts = 0, curr_d = 0;
	for (i = 1; i < Edge.rows - 1; i++) {
		for (j = 1; j < Edge.cols - 1; j++)
		{
			// 起始点及当前点
			//Point s_pt = Point(i, j);
			Point b_pt = Point(i, j);
			Point c_pt = Point(i, j);

			// 如果当前点为前景点
			if (255 == Edge.at<uchar>(c_pt.x, c_pt.y))
			{
				edge_t.clear();
				bool tra_flag = false;
				// 存入
				edge_t.push_back(c_pt);
				Edge.at<uchar>(c_pt.x, c_pt.y) = 0;    // 用过的点直接给设置为0

				// 进行跟踪
				while (!tra_flag)
				{
					// 循环八次
					for (counts = 0; counts < 8; counts++)
					{
						// 防止索引出界
						if (curr_d >= 8)
						{
							curr_d -= 8;
						}
						if (curr_d < 0)
						{
							curr_d += 8;
						}

						// 当前点坐标
						// 跟踪的过程，应该是个连续的过程，需要不停的更新搜索的root点
						c_pt = Point(b_pt.x + directions[curr_d].x, b_pt.y + directions[curr_d].y);

						// 边界判断
						if ((c_pt.x > 0) && (c_pt.x < Edge.rows - 1) &&
							(c_pt.y > 0) && (c_pt.y < Edge.cols - 1))
						{
							// 如果存在边缘
							if (255 == Edge.at<uchar>(c_pt.x, c_pt.y))
							{
								curr_d -= 2;   // 更新当前方向
								edge_t.push_back(c_pt);
								Edge.at<uchar>(c_pt.x, c_pt.y) = 0;

								// 更新b_pt:跟踪的root点
								b_pt.x = c_pt.x;
								b_pt.y = c_pt.y;

								//cout << c_pt.x << " " << c_pt.y << endl;

								break;   // 跳出for循环
							}
						}
						curr_d++;
					}   // end for
					// 跟踪的终止条件：如果8邻域都不存在边缘
					if (8 == counts)
					{
						// 清零
						curr_d = 0;
						tra_flag = true;
						edges.push_back(edge_t);

						break;
					}

				}  // end if
			}  // end while

		}
	}

	RNG rng(time(0));

	// 显示一下
	Mat trace_edge = Mat::zeros(Edge.rows, Edge.cols, CV_8UC1);
	Mat trace_edge_color;
	cvtColor(trace_edge, trace_edge_color, CV_GRAY2BGR);
	for (i = 0; i < edges.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

		//cout << edges[i].size() << endl;
		// 过滤掉较小的边缘
		if (edges[i].size() > 20)
		{
			for (j = 0; j < edges[i].size(); j++)
			{
				trace_edge_color.at<Vec3b>(edges[i][j].x, edges[i][j].y)[0] = 255;
				trace_edge_color.at<Vec3b>(edges[i][j].x, edges[i][j].y)[1] = 255;
				trace_edge_color.at<Vec3b>(edges[i][j].x, edges[i][j].y)[2] = 255;
			}
		}

	}
	return trace_edge_color;
}

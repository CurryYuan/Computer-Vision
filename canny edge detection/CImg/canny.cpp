#include "canny.h"
#define M_PI       acos(-1.0)

canny::canny(string filename)
{
	img.load_bmp(filename.c_str());
	if (!img.data()) // Check for invalid input
	{
		cout << "Could not open or find the image" << endl;
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

		img.save("./result/Original.bmp");

		grayscaled = toGrayScale(); //Grayscale the image
		grayscaled.save("./result/GrayScaled.bmp");

		gFiltered = useFilter(grayscaled, filter); //Gaussian Filter
		gFiltered.save("./result/Gaussian Blur.bmp");

		sFiltered = sobel(); //Sobel Filter
		sFiltered.save("./result/Sobel Filtered.bmp");

		non = nonMaxSupp();	//Non-Maxima Suppression
		non.save("./result/Non-Maxima Supp.bmp");

		thres = threshold(non, 40, 100);	//Double Threshold and Finalize
		thres.save("./result/Final.bmp");

		edge = edgeTrack(thres);
		edge.save("./result/Edge.bmp");
	}
}

CImg<uchar> canny::toGrayScale()
{
	grayscaled = CImg<uchar>(img.width(), img.height(), 1, 1);
	cimg_forXY(grayscaled, x, y) {
		int r = img(x, y, 0);
		int g = img(x, y, 1);
		int b = img(x, y, 2);

		double newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
		for (int i = 0; i < 3; ++i)
			grayscaled(x, y) = newValue;
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

	for (int x = -row / 2; x <= row / 2; x++)
	{
		for (int y = -column / 2; y <= column / 2; y++)
		{
			coordSum = (x*x + y * y);
			filter[x + row / 2][y + column / 2] = (exp(-(coordSum) / constant)) / (M_PI * constant);
			sum += filter[x + row / 2][y + column / 2];
		}
	}

	// Normalize the Filter
	for (int i = 0; i < row; i++)
		for (int j = 0; j < column; j++)
			filter[i][j] /= sum;

	return filter;
}

CImg<uchar> canny::useFilter(CImg<uchar> img_in, vector<vector<double>> filterIn)
{
	int size = (int)filterIn.size() / 2;
	CImg<uchar> filteredImg(img_in.width() - 2 * size, img_in.height() - 2 * size, 1, 1);
	for (int i = size; i < img_in.width() - size; i++)
	{
		for (int j = size; j < img_in.height() - size; j++)
		{
			double sum = 0;

			for (int x = 0; x < filterIn.size(); x++)
				for (int y = 0; y < filterIn.size(); y++)
				{
					sum += filterIn[x][y] * (double)(img_in(i + x - size, j + y - size));
				}

			filteredImg(i - size, j - size) = sum;
		}

	}
	return filteredImg;
}

CImg<uchar> canny::sobel()
{
	//Sobel X Filter
	double x1[] = { 1.0, 0, -1.0 };
	double x2[] = { 2.0, 0, -2.0 };
	double x3[] = { 1.0, 0, -1.0 };

	vector<vector<double>> xFilter(3);
	xFilter[0].assign(x1, x1 + 3);
	xFilter[1].assign(x2, x2 + 3);
	xFilter[2].assign(x3, x3 + 3);

	//Sobel Y Filter
	double y1[] = { -1.0, -2.0, -1.0 };
	double y2[] = { 0, 0, 0 };
	double y3[] = { 1.0, 2.0, 1.0 };

	vector<vector<double>> yFilter(3);
	yFilter[0].assign(y1, y1 + 3);
	yFilter[1].assign(y2, y2 + 3);
	yFilter[2].assign(y3, y3 + 3);

	//Limit Size
	int size = (int)xFilter.size() / 2;

	CImg<uchar> filteredImg(gFiltered.width() - 2 * size, gFiltered.height() - 2 * size, 1, 1);

	angles = CImg<float>(gFiltered.width() - 2 * size, gFiltered.height() - 2 * size, 1, 1); //AngleMap

	for (int i = size; i < gFiltered.width() - size; i++)
	{
		for (int j = size; j < gFiltered.height() - size; j++)
		{
			double sumx = 0;
			double sumy = 0;

			for (int x = 0; x < xFilter.size(); x++)
				for (int y = 0; y < yFilter.size(); y++)
				{
					sumx += yFilter[x][y] * (double)(gFiltered(i + x - size, j + y - size)); //Sobel_X Filter Value
					sumy += xFilter[x][y] * (double)(gFiltered(i + x - size, j + y - size)); //Sobel_Y Filter Value
				}
			double sumxsq = sumx * sumx;
			double sumysq = sumy * sumy;

			double sq2 = sqrt(sumxsq + sumysq);

			if (sq2 > 255) //Unsigned Char Fix
				sq2 = 255;
			filteredImg(i - size, j - size) = sq2;

			if (sumx == 0) //Arctan Fix
				angles(i - size, j - size) = 90;
			else
				angles(i - size, j - size) = atan(sumy / sumx)*(180.0 / M_PI);
		}
	}
	
	return filteredImg;
}

CImg<uchar> canny::nonMaxSupp()
{
	CImg<uchar> nonMaxSupped(sFiltered.width() - 2, sFiltered.height() - 2, 1, 1);
	for (int i = 1; i < sFiltered.width() - 1; i++) {
		for (int j = 1; j < sFiltered.height() - 1; j++) {
			float Tangent = angles(i, j);
			//cout << Tangent << " ";
			nonMaxSupped(i - 1, j - 1) = sFiltered(i, j);
			//Horizontal Edge
			if (((-22.5 < Tangent) && (Tangent <= 22.5)) || ((157.5 < Tangent) && (Tangent <= -157.5)))
			{
				if ((sFiltered(i, j) < sFiltered(i+1, j )) || (sFiltered(i, j) < sFiltered(i-1, j )))
					nonMaxSupped(i - 1, j - 1) = 0;
			}
			//Vertical Edge
			if (((-112.5 < Tangent) && (Tangent <= -67.5)) || ((67.5 < Tangent) && (Tangent <= 112.5)))
			{
				if ((sFiltered(i, j) < sFiltered(i , j+1)) || (sFiltered(i, j) < sFiltered(i , j-1)))
					nonMaxSupped(i - 1, j - 1) = 0;
			}

			//-45 Degree Edge
			if (((-67.5 < Tangent) && (Tangent <= -22.5)) || ((112.5 < Tangent) && (Tangent <= 157.5)))
			{
				//if ((sFiltered(i, j) < sFiltered(i - 1, j + 1)) || (sFiltered(i, j) < sFiltered(i + 1, j - 1)))
				if ((sFiltered(i, j) < sFiltered(i + 1, j + 1)) || (sFiltered(i, j) < sFiltered(i - 1, j - 1)))
					nonMaxSupped(i - 1, j - 1) = 0;
			}

			//45 Degree Edge
			if (((-157.5 < Tangent) && (Tangent <= -112.5)) || ((22.5 < Tangent) && (Tangent <= 67.5)))
			{
				//if ((sFiltered(i, j) < sFiltered(i + 1, j + 1)) || (sFiltered(i, j) < sFiltered(i - 1, j - 1)))
				if ((sFiltered(i, j) < sFiltered(i - 1, j + 1)) || (sFiltered(i, j) < sFiltered(i + 1, j - 1)))
					nonMaxSupped(i - 1, j - 1) = 0;
			}
		}
	}
	return nonMaxSupped;
}

CImg<uchar> canny::threshold(CImg<uchar> imgin, int low, int high)
{
	if (low > 255)
		low = 255;
	if (high > 255)
		high = 255;

	CImg<uchar> EdgeMat(imgin.width(), imgin.height(), 1, 1);

	for (int i = 0; i < imgin.width(); i++)
	{
		for (int j = 0; j < imgin.height(); j++)
		{
			EdgeMat(i, j) = imgin(i, j);
			if (EdgeMat(i, j) > high)
				EdgeMat(i, j) = 255;
			else if (EdgeMat(i, j) < low)
				EdgeMat(i, j) = 0;
			else
			{
				bool anyHigh = false;
				bool anyBetween = false;
				for (int x = i - 1; x < i + 2; x++)
				{
					for (int y = j - 1; y < j + 2; y++)
					{
						if (x <= 0 || y <= 0 || x >= EdgeMat.width() || y >= EdgeMat.height()) //Out of bounds
							continue;
						else
						{
							if (EdgeMat(x, y) > high)
							{
								EdgeMat(i, j) = 255;
								anyHigh = true;
								break;
							}
							else if (EdgeMat(x, y) <= high && EdgeMat(x, y) >= low)
								anyBetween = true;
						}
					}
					if (anyHigh)
						break;
				}
				if (!anyHigh && anyBetween)
					for (int x = i - 2; x < i + 3; x++)
					{
						for (int y = j - 1; y < j + 3; y++)
						{
							if (x < 0 || y < 0 || x >= EdgeMat.width() || y >= EdgeMat.height()) //Out of bounds
								continue;
							else
							{
								if (EdgeMat(x, y) > high)
								{
									EdgeMat(i, j) = 255;
									anyHigh = true;
									break;
								}
							}
						}
						if (anyHigh)
							break;
					}
				if (!anyHigh)
					EdgeMat(i, j) = 0;
			}
		}
	}
	return EdgeMat;
}

CImg<uchar> canny::edgeTrack(CImg<uchar> Edge)
{
	// 8 neighbors
	const Point directions[8] = { { 1, 0 }, {1,1}, { 0, 1 }, { -1, 1 }, { -1, 0 },  { -1, -1 }, { 0, -1 },{ 1, -1 } };

	vector<Point> edge_t;
	vector<vector<Point>> edges;

	// 边缘跟踪
	int i, j, counts = 0, curr_d = 0;
	for (i = 1; i < Edge.width() - 1; i++) {
		for (j = 1; j < Edge.height() - 1; j++)
		{
			// 起始点及当前点
			Point b_pt = Point(i, j);
			Point c_pt = Point(i, j);

			// 如果当前点为前景点
			if (255 == Edge(c_pt.x, c_pt.y))
			{
				edge_t.clear();
				bool tra_flag = false;
				// 存入
				edge_t.push_back(c_pt);
				Edge(c_pt.x, c_pt.y) = 0;    // 用过的点直接给设置为0

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
						if ((c_pt.x > 0) && (c_pt.x < Edge.width() - 1) &&
							(c_pt.y > 0) && (c_pt.y < Edge.height() - 1))
						{
							// 如果存在边缘
							if (255 == Edge(c_pt.x, c_pt.y))
							{
								curr_d -= 2;   // 更新当前方向
								edge_t.push_back(c_pt);
								Edge(c_pt.x, c_pt.y) = 0;

								// 更新b_pt:跟踪的root点
								b_pt = c_pt;

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

	CImg<uchar> trace_edge_color(Edge.width(), Edge.height(), 1, 1, 0);

	for (i = 0; i < edges.size(); i++)
	{
		// 过滤掉较小的边缘 
		if (edges[i].size() > 20)
		{
			for (j = 0; j < edges[i].size(); j++)
			{
				trace_edge_color(edges[i][j].x, edges[i][j].y) = 255;
			}
		}

	}

	return trace_edge_color;
}




#include "Hough.h"


Hough::Hough(string filename) {
	fileName = filename;
	img.load(filename.c_str());

	width = img.width();
	height = img.height();
	max_length = sqrt(pow(width, 2) + pow(height, 2));

	gFiltered = img.get_norm().normalize(0, 255);
	gFiltered.blur(5);
	Prewitt();

	houghSpaceTransform();
	drawLines();
	drawPoints();
	orderVertexs();

	resultImg.save("res.bmp");

}

void Hough::Prewitt()
{
	gradnum = CImg<double>(img.width(), img.height(), 1, 1, 0);
	// 定义3*3领域矩阵I  
	CImg_3x3(I, double);
	// 遍历计算梯度值  
	cimg_for3x3(gFiltered, x, y, 0, 0, I, double) {
		const double ix = Inc - Ipc;
		const double iy = Icp - Icn;
		double grad = std::sqrt(ix*ix + iy * iy);
		// 梯度大于阈值才赋值  
		if (grad > gradLimit) {
			gradnum(x, y) = grad;
		}
	}
	//gradnum.display();
}

void Hough::houghSpaceTransform()
{
	double w = width;
	double h = height;
	double center_x = w / 2;
	double center_y = h / 2;
	double hough_h = ((sqrt(2.0) * (double)(h > w ? h : w)) / 2.0);
	houghImage.resize(180, hough_h * 2, 1, 1, 0);
	cimg_forXY(gradnum, x, y) {
		if (gradnum(x, y) != 0) {
			cimg_forX(houghImage, angle) {
				double _angle = (double)interval*angle;
				int polar = (int)((((double)x - center_x)*cos(_angle) + ((double)y - center_y)*sin(_angle)) + hough_h);
				//cout << polar << endl;
				houghImage(angle, polar) += 1;
			}
		}
	}
}

double Hough::distance(double x, double y) {
	return sqrt(x*x + y * y);
}

void Hough::drawLines()
{
	resultImg = img;
	//剔除掉可能出现的重合线，方法是取9x9空间内的霍夫最大值
	int hough_h = houghImage._height;
	//int hough_w = houghImg._width;
	int img_h = height;
	int img_w = width;
	const int y_min = 0;
	const int y_max = height - 1;
	const int x_min = 0;
	const int x_max = width - 1;
	cimg_forXY(houghImage, angle, polar) {
		if (houghImage(angle, polar) >= min_votes) {
			int max = houghImage(angle, polar);
			for (int ly = -DIFF; ly <= DIFF; ly++) {
				for (int lx = -DIFF; lx <= DIFF; lx++) {
					if ((ly + polar >= 0 && ly + polar < houghImage._height) && (lx + angle >= 0 && lx + angle < houghImage._width)) {
						if ((int)houghImage(angle + lx, polar + ly) > max) {
							max = houghImage(angle + lx, polar + ly);
							ly = lx = DIFF + 1;
						}
					}
				}
			}
			if (max > (int)houghImage(angle, polar))
				continue;
			peaks.push_back(votePoint(Point(angle, polar), houghImage(angle, polar)));
		}
	}
	sort(peaks.begin(), peaks.end(), [](const votePoint &a, const votePoint &b) -> int {return a.vote > b.vote; });
	for (int i = 0; lines.size() != 4; i++) {
		int angle = peaks[i].p.x;
		int polar = peaks[i].p.y;
		//cout << angle << endl << polar << endl;
		int x1, y1, x2, y2;
		x1 = y1 = x2 = y2 = 0;
		double _angle = interval*angle;
		if (angle >= 45 && angle <= 135) {
			x1 = 0;
			y1 = ((double)(polar - (hough_h / 2)) - ((x1 - (img_w / 2)) * cos(_angle))) / sin(_angle) + (img_h / 2);
			x2 = img_w;
			y2 = ((double)(polar - (hough_h / 2)) - ((x2 - (img_w / 2)) * cos(_angle))) / sin(_angle) + (img_h / 2);
		}
		else {
			y1 = 0;
			x1 = ((double)(polar - (hough_h / 2)) - ((y1 - (img_h / 2)) * sin(_angle))) / cos(_angle) + (img_w / 2);
			y2 = img_h;
			x2 = ((double)(polar - (hough_h / 2)) - ((y2 - (img_h / 2)) * sin(_angle))) / cos(_angle) + (img_w / 2);
		}
		//if
		bool flag = true;
		for (int k = 0; k < lines.size(); k++) {
			if (distance(lines[k].first.first - x1, lines[k].first.second - y1) < 100 && distance(lines[k].second.first - x2, lines[k].second.second - y2) < 100) {
				flag = false;
				break;
			}
		}
		if (flag == true) {
			lines.push_back(pair< pair<int, int>, pair<int, int> >(pair<int, int>(x1, y1), pair<int, int>(x2, y2)));
		}
	}

	for (int i = 0; i < lines.size(); i++) {
		cout << lines[i].first.first << ", " << lines[i].first.second << "  ..  " << lines[i].second.first << ", " << lines[i].second.second << endl;
		resultImg.draw_line(lines[i].first.first, lines[i].first.second, lines[i].second.first, lines[i].second.second, Red);
	}
}

void Hough::drawPoints()
{
	for (int i = 0; i < lines.size(); i++) {
		double k0, b0;
		if (lines[i].first.first == lines[i].second.first) {
			k0 = DBL_MAX;
			b0 = lines[i].first.first;
		}
		else {
			k0 = (double)(lines[i].first.second - lines[i].second.second) / (lines[i].first.first - lines[i].second.first);
			b0 = (double)(lines[i].first.second * lines[i].second.first - lines[i].second.second * lines[i].first.first) / (lines[i].second.first - lines[i].first.first);
		}
		for (int j = i + 1; j < lines.size(); j++) {
			double k1, b1;
			if (lines[j].first.first == lines[j].second.first) {
				k1 = DBL_MAX;
				b1 = lines[j].first.first;
			}
			else {
				k1 = (double)(lines[j].first.second - lines[j].second.second) / (lines[j].first.first - lines[j].second.first);
				b1 = (double)(lines[j].first.second * lines[j].second.first - lines[j].second.second * lines[j].first.first) / (lines[j].second.first - lines[j].first.first);
			}
			if (k0 == k1)
				continue;
			if (k0 == DBL_MAX) {
				int _x = b0, _y = k1 * b0 + b1;
				if (_x >= 0 && _x < width && _y >= 0 && _y < height)
					vertex.push_back(make_pair(_x, _y));
				continue;
			}
			if (k1 == DBL_MAX) {
				int _x = b1, _y = k0 * b1 + b0;
				if (_x >= 0 && _x < width && _y >= 0 && _y < height)
					vertex.push_back(make_pair(_x, _y));
				continue;
			}
			int _x = (b0 - b1) / (k1 - k0);
			int _y = (k0 * b1 - k1 * b0) / (k0 - k1);
			if (_x >= 0 && _x < width && _y >= 0 && _y < height)
				vertex.push_back(make_pair(_x, _y));
		}
	}
	for (int i = 0; i < vertex.size(); i++) {
		resultImg.draw_circle(vertex[i].first, vertex[i].second, 5, Red);
	}
}

void Hough::orderVertexs() {
	sort(vertex.begin(), vertex.end(), [](const pair<int, int> &a, const pair<int, int> &b)-> int {return a.second<b.second; });
	if (vertex[0].first > vertex[1].first) {
		swap(vertex[0], vertex[1]);
	}
	if (vertex[2].first > vertex[3].first) {
		swap(vertex[2], vertex[3]);
	}

	ofstream output;
	output.open("point.csv", ios::app);
	output << fileName << ",";
	for (int i = 0; i < vertex.size(); i++) {
		cout << "(" << vertex[i].first << "  " << vertex[i].second << ")" << endl;
		output << "(" << vertex[i].first << "  " << vertex[i].second << ")" << ",";
	}
	output << endl;
}
#include "Hough.h"


Hough::Hough(string filename) {
	img.load(filename.c_str());

	width = img.width();
	height = img.height();
	max_length = sqrt(pow(width, 2) + pow(height, 2));

	/*gFiltered = img.get_norm().normalize(0, 255);
	gFiltered.blur(5);
	Prewitt();*/
	//sobel();
	gradnum = img;
	houghSpaceTransform();
	houghLinesDetect();
	drawLines();
	drawPoints();
	result.save("res.bmp");


	/*houghCircleTransform();
	result.save("./result2/2.bmp");*/

}

void Hough::houghSpaceTransform()
{
	houghImage = CImg<int>(theta, max_length, 1, 1, 0);

	cimg_forXY(gradnum, x, y) {
		int temp = gradnum(x, y);
		if (temp != 0&&y>2&&y<1000) {
			for (int i = 0; i < theta; ++i) {
				double r = x * cos(i*interval) + y * sin(i*interval);
				if (r >= 0 && r < max_length) {
					houghImage(i, r)++;		//voting
				}
			}
		}
	}
}

double Hough::distance(double x, double y) {
	return sqrt(x*x + y * y);
}

void Hough::houghLinesDetect()
{
	cimg_forXY(houghImage, x, y) {
		if (houghImage(x, y) > min_votes) {
			bool flag = false;
			for (auto& c : peaks) {
				if (distance(c.x - x, c.y - y) < min_distance) {
					flag = true;
					if (houghImage(x, y) > houghImage(c.x, c.y)) {
						c = Point(x, y);
					}
				}
			}
			if (!flag) {
				peaks.push_back(Point(x, y));
			}
		}
	}
	cout << peaks.size() << endl;

}

void Hough::drawLines()
{
	lines.clear();
	for (int i = 0; i < peaks.size(); i++) {
		double theta = double(peaks[i].x)*interval;
		double k = -cos(theta) / sin(theta); // 直线斜率
		if (peaks[i].x == 90 || peaks[i].x == 270)
			k = 0;
		double b = double(peaks[i].y) / sin(theta);
		Line templine(k, b);
		lines.push_back(templine);
	}
	int x_min = 0, y_min = 0;
	int x_max = img.width() - 1;
	int y_max = img.height() - 1;
	result = img;

	const double lines_color[] = { 0, 0, 255 };
	for (int i = 0; i < lines.size(); i++) {
		const int x0 = (y_min - lines[i].b) / lines[i].k;
		const int x1 = (y_max - lines[i].b) / lines[i].k;
		const int x = peaks[i].y / cos(peaks[i].x*interval);
		const int y = peaks[i].y / sin(peaks[i].x*interval);

		if (peaks[i].x > 0 && peaks[i].x <= 360) {	//斜率存在
			if (lines[i].k != 0) {
				result.draw_line(x0, y_min, x1, y_max, lines_color);
				//cout << x0 << " " << y_min << " " << x1 << " " << y_max << endl;
			}
			else {	//斜率为0
				result.draw_line(x_min, y, x_max, y, lines_color);
			}
		}
		else {		//斜率不存在
			result.draw_line(x, y_min, x, y_max, lines_color);

			//cout << y_min << " " << x << endl;
		}
	}
}

void Hough::drawPoints()
{
	for (int i = 0; i < peaks.size(); ++i) {
		cout << "vote: " << houghImage(peaks[i].x, peaks[i].y) << endl;
		cout << "angle: " << peaks[i].x << " " << peaks[i].y << endl;

		double angle = peaks[i].x*interval;
		double a0 = cos(angle), b0 = sin(angle), c0 = -peaks[i].y;
		//cout <<"Peaks "<< peaks[i].x << " "<<peaks[i].y << endl;
		if (peaks[i].x == 90)
			a0 = 0;
		cout << "Line" << i << ": ( " << a0 << " ) x + (" << b0 << ") y + ( " << c0 << " ) = 0" << endl;

		for (int j = i + 1; j < peaks.size(); ++j) {
			double angle1 = peaks[j].x*interval;
			double a1 = cos(angle1), b1 = sin(angle1), c1 = -peaks[j].y;
			if (peaks[j].x == 90 || peaks[j].x == 270)
				a1 = 0;
			double D = a0 * b1 - a1 * b0;
			//cout << "D " << D <<" "<< a0 * b1 << " " << a1 * b0 << endl;
			if (D != 0) {
				int x = (b0*c1 - b1 * c0) / D;
				int y = (a1*c0 - a0 * c1) / D;
				//cout <<"P"<< (b0*c1 - b1 * c0) / D << endl;
				if (x > 0 && x < width - 1 && y>0 && y < height - 1) {

					points.push_back(Point(x, y));
				}
			}
		}
	}

	cout << "Points:" << endl;

	for (auto c : points) {
		const double color[] = { 255,0,0 };
		cout << "( " << c.x << " , " << c.y << " )" << endl;
		result.draw_circle(c.x, c.y, 5, color);
	}
	result.display();
}

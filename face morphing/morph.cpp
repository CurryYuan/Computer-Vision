#include "morph.h"

// 判断点p是否是三角形的顶点
bool triangle::containsVertex(const point &p) {
	return a == p || b == p || c == p;
}

morph::morph()
{
	// --- step 1 : read img ---

	string filename_A = "dataset/1.jpg";
	string filename_B = "dataset/2.bmp";
	CImg<float> img_A(filename_A.c_str());
	CImg<float> img_B(filename_B.c_str());

	// --- step 2 : get dection points ---
	vector<point*> points_A, points_B;

	// add four corners and four middle points
	points_A.push_back(new point(0, 0));
	points_A.push_back(new point(0, img_A._height));
	points_A.push_back(new point(img_A._width, 0));
	points_A.push_back(new point(img_A._width, img_A._height));
	points_A.push_back(new point(0, img_A._height / 2));
	points_A.push_back(new point(img_A._width / 2, 0));
	points_A.push_back(new point(img_A._width, img_A._height / 2));
	points_A.push_back(new point(img_A._width / 2, img_A._height));

	points_B.push_back(new point(0, 0));
	points_B.push_back(new point(0, img_B._height));
	points_B.push_back(new point(img_B._width, 0));
	points_B.push_back(new point(img_B._width, img_B._height));
	points_B.push_back(new point(0, img_B._height / 2));
	points_B.push_back(new point(img_B._width / 2, 0));
	points_B.push_back(new point(img_B._width, img_B._height / 2));
	points_B.push_back(new point(img_B._width / 2, img_B._height));

	getDetectionPoints(points_A, img_A, filename_A);
	getDetectionPoints(points_B, img_B, filename_B);

	if (points_A.size() != points_B.size()) {
		throw ("the decetion points size are no the same");
	}

	for (int i = 0; i < points_A.size(); ++i) {
		points_A[i]->index = i;
	}

	// --- step 3 : get Delaunay triangles --- 
	vector<triangle*> triangles_A, triangles_B;
	getDelaunayTriangles(triangles_A, points_A);

	// get couterpart delaunay triangles of B from A
	for (int i = 0; i < triangles_A.size(); i++) {
		triangles_B.push_back(new triangle(*points_B.at(triangles_A[i]->index[0]),
			*points_B.at(triangles_A[i]->index[1]),
			*points_B.at(triangles_A[i]->index[2]),
			triangles_A[i]->index[0],
			triangles_A[i]->index[1],
			triangles_A[i]->index[2]));
	}
	drawTriangles(triangles_A, img_A);
	drawTriangles(triangles_B, img_B);

	// --- step 4: morph ---
	double rate = 0;
	for (int i = 0; i <= 10; i++) {
		CImg<float> frame = morphTriangle(rate, triangles_A, triangles_B, img_A, img_B);
		rate += 0.1;
		string filename = "result/.jpg";

		filename.insert(7, std::to_string(i));

		frame.save(filename.c_str());
		cout << "saving frame " << i << endl;
	}
}


/*
* get dection points to vector<point> manually
* this function support to reuse the records we have record in fliename .txt
* if you want to detect the points by yourself just remove the txt file
* record method from outer loop in inner loop
* in this example 24 points each picture
*/
CImg<float> morph::getDetectionPoints(vector<point*>& points, const CImg<float>& src, const string filename) {
	// 1.jpg to 1.txt which record the points
	string points_file(filename);
	points_file.replace(points_file.end() - 3, points_file.end(), "txt");
	CImg<float> dect_img = src;
	int color[] = { 255, 0, 0 };

	// try to read detection points from file
	ifstream input(points_file);
	ofstream output;
	if (input.fail()) { // if no exist points
		output.open(points_file);
		CImgDisplay disp(dect_img, filename.c_str());

		while (!disp.is_closed()) {
			disp.wait();
			if (disp.button() & 1 && disp.mouse_y() >= 0) {
				point* p = new point(disp.mouse_x(), disp.mouse_y());
				points.push_back(p);
				// draw circle
				dect_img.draw_circle(p->x, p->y, dect_img._width / 100, color);
				dect_img.display(disp);
				// write file
				output << p->x << "," << p->y << endl;
			}
		}
		output.close();
	}
	else {
		string line;
		while (getline(input, line)) {
			// point format : x,y
			int pos = line.find(',');
			if (pos != string::npos) {
				// skip ","
				string x_str = line.substr(0, pos);
				string y_str = line.substr(pos + 1, string::npos);
				point* p = new point(std::stoi(x_str), std::stoi(y_str));
				points.push_back(p);
				// draw circle
				dect_img.draw_circle(p->x, p->y, dect_img._width / 100, color);
			}
		}
		dect_img.display();
	}
	input.close();
	return dect_img;
}

double morph::getDistance(point A, point B) {
	double dis = (A.x - B.x)*(A.x - B.x) + (A.y - B.y)*(A.y - B.y);
	return sqrt(dis);
}

// to judge is point p in the circumcircle of tri
// formular find in wikipedia Circumscribed circle
bool morph::isPointInCircle(point* P, triangle* tri) {
	point origin(0, 0);
	double a_x = tri->a.x;
	double b_x = tri->b.x;
	double c_x = tri->c.x;
	double a_y = tri->a.y;
	double b_y = tri->b.y;
	double c_y = tri->c.y;
	double D = 2 * (a_x*(b_y - c_y) + b_x * (c_y - a_y) + c_x * (a_y - b_y));
	double x = ((a_x*a_x + a_y * a_y)*(b_y - c_y) + (b_x*b_x + b_y * b_y)*(c_y - a_y) + (c_x*c_x + c_y * c_y)*(a_y - b_y)) / D;
	double y = ((a_x*a_x + a_y * a_y)*(c_x - b_x) + (b_x*b_x + b_y * b_y)*(a_x - c_x) + (c_x*c_x + c_y * c_y)*(b_x - a_x)) / D;
	point center((int)x, (int)y);
	double radius = getDistance(center, tri->a);

	return (getDistance(*P, center) <= radius);
}

// 三角剖分
// Bowyer-Watson算法
void morph::getDelaunayTriangles(vector<triangle*>& triangles, const vector<point*>& points) {
	// 求超级三角形
	float minX = points[0]->x;
	float minY = points[0]->y;
	float maxX = minX;
	float maxY = minY;

	for (auto p : points) {
		if (p->x < minX) minX = p->x;
		if (p->y < minY) minY = p->y;
		if (p->x > maxX) maxX = p->x;
		if (p->y > maxY) maxY = p->y;
	}

	float dx = maxX - minX, dy = maxY - minY;
	float deltaMax = max(dx, dy);
	float midx = (minX + maxX) / 2, midy = (minY + maxY) / 2;

	point p1(midx - 20 * deltaMax, midy - deltaMax);
	point p2(midx, midy + 20 * deltaMax);
	point p3(midx + 20 * deltaMax, midy - deltaMax);

	// 将超级三角形加入三角形列表
	triangles.push_back(new triangle(p1, p2, p3));

	for (point* p : points) {
		vector<triangle*> badTriangles;
		vector<Edge> polygon;

		for (triangle* t : triangles) {
			if (isPointInCircle(p,t)) {
				badTriangles.push_back(t);
				polygon.push_back(t->e1);
				polygon.push_back(t->e2);
				polygon.push_back(t->e3);
			}
		}

		triangles.erase(remove_if(begin(triangles), end(triangles), [badTriangles](triangle* t) {
			for (triangle* bt : badTriangles) {
				if (*bt == *t) return true;
			}
			return false;
		}), end(triangles));

		vector<Edge> badEdges;
		int size = polygon.size();
		for (int i = 0; i < size; ++i) {
			for (int j = i + 1; j < size; ++j) {
				if (polygon[i] == polygon[j]) {
					badEdges.push_back(polygon[i]);
					badEdges.push_back(polygon[j]);
				}
			}
		}

		polygon.erase(remove_if(begin(polygon), end(polygon), [badEdges](Edge &e) {
			for (Edge be : badEdges) {
				if (be == e) return true;
			}
			return false;
		}), end(polygon));

		for (Edge e : polygon)
			triangles.push_back(new triangle(e.a, e.b, *p,e.a.index,e.b.index,p->index));
	}

	triangles.erase(remove_if(begin(triangles), end(triangles), [p1, p2, p3](triangle* t) {
		return t->containsVertex(p1) || t->containsVertex(p2) || t->containsVertex(p3);
	}), end(triangles));
}

// draw triangle lines
void morph::drawTriangles(const vector<triangle*>& triangles, const CImg<float>& src) {
	CImg<float> disp(src);
	int color[] = { 255, 0, 0 };
	for (int i = 0; i < triangles.size(); i++) {
		disp.draw_line(triangles[i]->a.x, triangles[i]->a.y,
			triangles[i]->b.x, triangles[i]->b.y, color);
		disp.draw_line(triangles[i]->a.x, triangles[i]->a.y,
			triangles[i]->c.x, triangles[i]->c.y, color);
		disp.draw_line(triangles[i]->b.x, triangles[i]->b.y,
			triangles[i]->c.x, triangles[i]->c.y, color);
		disp.draw_circle(triangles[i]->a.x, triangles[i]->a.y, disp._width / 100, color);
		disp.draw_circle(triangles[i]->b.x, triangles[i]->b.y, disp._width / 100, color);
		disp.draw_circle(triangles[i]->c.x, triangles[i]->c.y, disp._width / 100, color);
	}
	disp.display();
}

// transition between source A and B
triangle* morph::getTransitionTriangle(const triangle* A, const triangle* B, double rate) {
	int ax = (int)(rate*(A->a.x) + (1 - rate)*(B->a.x));
	int ay = (int)(rate*(A->a.y) + (1 - rate)*(B->a.y));
	int bx = (int)(rate*(A->b.x) + (1 - rate)*(B->b.x));
	int by = (int)(rate*(A->b.y) + (1 - rate)*(B->b.y));
	int cx = (int)(rate*(A->c.x) + (1 - rate)*(B->c.x));
	int cy = (int)(rate*(A->c.y) + (1 - rate)*(B->c.y));
	return new triangle(point(ax, ay), point(bx, by), point(cx, cy));
}

CImg<float> morph::getTransTriangle2Triangle(const triangle* src, const triangle* dst) {

	// transform src to dst
	// !!! CImg 下标是先y后x !!!
	CImg<float> m1(3, 3);
	m1(0, 0) = src->a.x;
	m1(1, 0) = src->b.x;
	m1(2, 0) = src->c.x;
	m1(0, 1) = src->a.y;
	m1(1, 1) = src->b.y;
	m1(2, 1) = src->c.y;
	m1(0, 2) = m1(1, 2) = m1(2, 2) = 1;

	CImg<float> m2(3, 3);
	m2(0, 0) = dst->a.x;
	m2(1, 0) = dst->b.x;
	m2(2, 0) = dst->c.x;
	m2(0, 1) = dst->a.y;
	m2(1, 1) = dst->b.y;
	m2(2, 1) = dst->c.y;
	m2(0, 2) = m2(1, 2) = m2(2, 2) = 1;

	return m2 / m1;
}

int cross3(const point &a, const point &b, const point &p) {
	return (b.x - a.x)*(p.y - a.y) - (b.y - a.y)*(p.x - a.x);
}

bool triangle::isInTriangle(const point& p) {

	if (cross3(a, b, p) >= 0 && cross3(b, c, p) >= 0 && cross3(c, a, p) >= 0)
		return true;
	else if (cross3(a, b, p) <= 0 && cross3(b, c, p) <= 0 && cross3(c, a, p) <= 0)
		return true;
	else
		return false;
}

CImg<float> morph::morphTriangle(double rate, const vector<triangle*>& triangles_A, const vector<triangle*>& triangles_B,
	const CImg<float>& img_A, const CImg<float>& img_B)
{
	int width = (rate * img_A._width + (1 - rate) * img_B._width);
	int height = (rate * img_A._height + (1 - rate) * img_B._height);
	CImg<float> result(width, height, 1, 3);

	for (int i = 0; i < triangles_A.size(); i++) {
		triangle* trans_tri = getTransitionTriangle(triangles_A[i], triangles_B[i], rate);

		auto H1 = getTransTriangle2Triangle(trans_tri, triangles_A[i]);
		auto H2 = getTransTriangle2Triangle(trans_tri, triangles_B[i]);
		cimg_forXY(result, x, y) {
			if (trans_tri->isInTriangle(point(x, y))) {

				float tx_a = x * H1(0, 0) + y * H1(1, 0) + H1(2, 0);
				float ty_a = x * H1(0, 1) + y * H1(1, 1) + H1(2, 1);
				float pixel_a[3] = { 0 };
				cimg_forC(img_A, c) {
					pixel_a[c] = img_A.linear_atXY(tx_a, ty_a, 0, c);
				}

				float tx_b = x * H2(0, 0) + y * H2(1, 0) + H2(2, 0);
				float ty_b = x * H2(0, 1) + y * H2(1, 1) + H2(2, 1);
				float pixel_b[3] = { 0 };
				cimg_forC(img_B, c) {
					pixel_b[c] = img_B.linear_atXY(tx_b, ty_b, 0, c);
				}

				// morph
				cimg_forC(result, c) {
					result(x, y, 0, c) = rate * pixel_a[c] + (1 - rate)*pixel_b[c];
				}
			}

		}
	}

	return result;
}
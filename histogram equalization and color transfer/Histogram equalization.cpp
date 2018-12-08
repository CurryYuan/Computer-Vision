#include "CImg.h"
#include <cmath>
#include <iostream>
using namespace cimg_library;
using namespace std;
typedef unsigned char uchar;

CImg<uchar> histeq(CImg<uchar> gray) {
	float grayPixel[256] = { 0 };

	cimg_forXY(gray, x, y) {
		grayPixel[gray(x, y)]++;
	}

	for (int i = 0; i < 256; ++i) {
		grayPixel[i] = grayPixel[i] / gray.size();
	}

	for (int i = 1; i < 256; ++i) {
		grayPixel[i] += grayPixel[i - 1];
	}

	for (int i = 0; i < 256; ++i) {
		grayPixel[i] = round(grayPixel[i] * 255);
		cout << grayPixel[i] << " ";
	}

	cimg_forXY(gray, x, y) {
		gray(x, y) = grayPixel[gray(x, y)];
	}
	return gray;
}

int main() {
	CImg<uchar> img;
	img.load("result1/picture5/original.jpg");
	CImg<uchar> gray = img.get_norm().normalize(0, 255);
	
	gray = histeq(gray);

	gray.save("result1/picture5/gray.jpg");

	for (int i = 0; i < 3; ++i) {
		auto tempImage = img.get_shared_channel(i);
		tempImage = histeq(tempImage);
		img.get_shared_channel(i) = tempImage;
	}

	img.save("result1/picture5/color1.jpg");

	auto HSVImage = img.get_RGBtoHSV();
	auto tempImage = HSVImage.get_channel(2).get_norm().normalize(0, 255);

	tempImage = histeq(tempImage).get_normalize(0, 1);


	HSVImage.get_shared_channel(2) = tempImage;

	auto reImage = HSVImage.get_HSVtoRGB();
	reImage.save("result1/picture5/color2.jpg");
}
#include "Blending.h"

Blending::Blending() {
}

Blending::~Blending() {
}

Blending::Blending(int sx, int sy) {
	matchVec.dx = sx;
	matchVec.dy = sy;
}

void Blending::blendingMainProcess(const char* _filenameA, const char* _filenameB) {
	imgA.load(_filenameA);
	imgB.load(_filenameB);

	blendedImg = CImg<int>(imgA._width + imgB._width - matchVec.dx, 
		imgA._height + abs(matchVec.dy), 1, 3, 0);

	cimg_forXY(blendedImg, x, y) {
		if (matchVec.dy <= 0) {    //右侧图片需要往下左移动
			if (x < imgA._width && y < imgA._height) {
				if (x >= (imgA._width - matchVec.dx) && y >= (0 - matchVec.dy)) {    //混合
					blendedImg(x, y, 0, 0) = (float)imgA(x, y, 0, 0)
						* (float)(imgA._width - x) / (float)abs(matchVec.dx)
						+ (float)imgB(x - (imgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 0)
						* (float)(x - (imgA._width - matchVec.dx)) / (float)abs(matchVec.dx);
					blendedImg(x, y, 0, 1) = (float)imgA(x, y, 0, 1)
						* (float)(imgA._width - x) / (float)abs(matchVec.dx)
						+ (float)imgB(x - (imgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 1)
						* (float)(x - (imgA._width - matchVec.dx)) / (float)abs(matchVec.dx);
					blendedImg(x, y, 0, 2) = (float)imgA(x, y, 0, 2)
						* (float)(imgA._width - x) / (float)abs(matchVec.dx)
						+ (float)imgB(x - (imgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 2)
						* (float)(x - (imgA._width - matchVec.dx)) / (float)abs(matchVec.dx);
				}
				else {    //A独在部分
					blendedImg(x, y, 0, 0) = imgA(x, y, 0, 0);
					blendedImg(x, y, 0, 1) = imgA(x, y, 0, 1);
					blendedImg(x, y, 0, 2) = imgA(x, y, 0, 2);
				}
			}
			else if (x >= (imgA._width - matchVec.dx) 
				&& y >= (0 - matchVec.dy) && y < (0 - matchVec.dy) + imgB._height) {    //B独在部分
				blendedImg(x, y, 0, 0) = imgB(x - (imgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 0);
				blendedImg(x, y, 0, 1) = imgB(x - (imgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 1);
				blendedImg(x, y, 0, 2) = imgB(x - (imgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 2);
			}
			else {    //黑色部分
				blendedImg(x, y, 0, 0) = 0;
				blendedImg(x, y, 0, 1) = 0;
				blendedImg(x, y, 0, 2) = 0;
			}
		}
		else {    //matchVec.dy > 0; 右侧图片需要往上左移动
			if (x < imgA._width && y >= matchVec.dy) {
				if (x >= (imgA._width - matchVec.dx) && y < imgB._height) {    //混合
					blendedImg(x, y, 0, 0) = (float)imgA(x, y - matchVec.dy, 0, 0)
						* (float)(imgA._width - x) / (float)abs(matchVec.dx)
						+ (float)imgB(x - (imgA._width - matchVec.dx), y, 0, 0)
						* (float)(x - (imgA._width - matchVec.dx)) / (float)abs(matchVec.dx);
					blendedImg(x, y, 0, 1) = (float)imgA(x, y - matchVec.dy, 0, 1)
						* (float)(imgA._width - x) / (float)abs(matchVec.dx)
						+ (float)imgB(x - (imgA._width - matchVec.dx), y, 0, 1)
						* (float)(x - (imgA._width - matchVec.dx)) / (float)abs(matchVec.dx);
					blendedImg(x, y, 0, 2) = (float)imgA(x, y - matchVec.dy, 0, 2)
						* (float)(imgA._width - x) / (float)abs(matchVec.dx)
						+ (float)imgB(x - (imgA._width - matchVec.dx), y, 0, 2)
						* (float)(x - (imgA._width - matchVec.dx)) / (float)abs(matchVec.dx);
				}
				else {    //A独在部分
					blendedImg(x, y, 0, 0) = imgA(x, y - matchVec.dy, 0, 0);
					blendedImg(x, y, 0, 1) = imgA(x, y - matchVec.dy, 0, 1);
					blendedImg(x, y, 0, 2) = imgA(x, y - matchVec.dy, 0, 2);
				}
			}
			else if (x >= (imgA._width - matchVec.dx) && y < imgB._height) {    //B独在部分
				blendedImg(x, y, 0, 0) = imgB(x - (imgA._width - matchVec.dx), y, 0, 0);
				blendedImg(x, y, 0, 1) = imgB(x - (imgA._width - matchVec.dx), y, 0, 1);
				blendedImg(x, y, 0, 2) = imgB(x - (imgA._width - matchVec.dx), y, 0, 2);
			}
			else {    //黑色部分
				blendedImg(x, y, 0, 0) = 0;
				blendedImg(x, y, 0, 1) = 0;
				blendedImg(x, y, 0, 2) = 0;
			}
		}
	}
	blendedImg.display("blendedImg");
}


void Blending::saveBlendedImg(const char* blendedImgAddr) {
	blendedImg.save(blendedImgAddr);
}
#include "Matching.h"

Matching::Matching() {
}

Matching::~Matching() {
}

Matching::Matching(int _kp_count_A, Keypoint _firstKeyDesc_A, int _kp_count_B, Keypoint _firstKeyDesc_B) {
	keypoint_count_A = _kp_count_A;
	keypoint_count_B = _kp_count_B;
	firstKeyDescriptor_A = _firstKeyDesc_A;
	firstKeyDescriptor_B = _firstKeyDesc_B;
}

void Matching::featureMatching() {
	Keypoint tempDescA = firstKeyDescriptor_A;
	while (tempDescA) {
		float colA = tempDescA->col;
		float rowA = tempDescA->row;
		float* kp_desc_A = tempDescA->descrip;

		Keypoint tempDescB = firstKeyDescriptor_B;

		float minSSD = 100.0;
		int minIndex = -1;
		int colB = -1;
		int rowB = -1;
		while (tempDescB) {    //对A图每个点，找B图各个点，计算距离
			float ssd = 0;
			for (int i = 0; i < LEN; i++) {
				float descA = *(kp_desc_A + i);
				float descB = *(tempDescB->descrip + i);
				ssd += abs(descA - descB);
			}
			if (ssd < minSSD) {
				minSSD = ssd;
				colB = tempDescB->col;
				rowB = tempDescB->row;
			}
			tempDescB = tempDescB->next;
		}

		if (minSSD < FeatureDescGap) {    //当距离小于阈值，即当作一对匹配点
			Point pa(tempDescA->col, tempDescA->row);
			Point pb(colB, rowB);

			MatchedPair mpair(pa, pb, minSSD);
			matchedPairSet.push_back(mpair);
		}
		tempDescA = tempDescA->next;
	}

	for (int i = 0; i < matchedPairSet.size(); i++) {
		cout << "A col: " << matchedPairSet[i].keyPointA.col << ", row: " << matchedPairSet[i].keyPointA.row << endl;
		cout << " with B col: " << matchedPairSet[i].keyPointB.col << ", row: " << matchedPairSet[i].keyPointB.row << " , minSSD: " << matchedPairSet[i].minDis << endl;
	}
	cout << ">>> matchedPairSet.size: " << matchedPairSet.size() << endl;
}

void Matching::drawKeyPoint(const char* _filenameA, const char* _filenameB, const char* _saveAddrA, const char* _saveAddrB) {
	imgA.load(_filenameA);
	srcImgWithKpA = CImg<int>(imgA._width, imgA._height, 1, 3, 0);
	cimg_forXY(srcImgWithKpA, x, y) {
		srcImgWithKpA(x, y, 0, 0) = imgA(x, y, 0, 0);
		srcImgWithKpA(x, y, 0, 1) = imgA(x, y, 0, 1);
		srcImgWithKpA(x, y, 0, 2) = imgA(x, y, 0, 2);
	}

	imgB.load(_filenameB);
	srcImgWithKpB = CImg<int>(imgB._width, imgB._height, 1, 3, 0);
	cimg_forXY(srcImgWithKpB, x, y) {
		srcImgWithKpB(x, y, 0, 0) = imgB(x, y, 0, 0);
		srcImgWithKpB(x, y, 0, 1) = imgB(x, y, 0, 1);
		srcImgWithKpB(x, y, 0, 2) = imgB(x, y, 0, 2);
	}

	const double yellow[] = { 255, 255, 0 };
	for (int i = 0; i < matchedPairSet.size(); i++) {
		cout << "A col: " << matchedPairSet[i].keyPointA.col << ", row: " << matchedPairSet[i].keyPointA.row << endl;
		cout << " with B col: " << matchedPairSet[i].keyPointB.col << ", row: " << matchedPairSet[i].keyPointB.row << " , minSSD: " << matchedPairSet[i].minDis << endl;
		srcImgWithKpA.draw_circle(matchedPairSet[i].keyPointA.col, matchedPairSet[i].keyPointA.row, 3, yellow, 1.0f);
		srcImgWithKpB.draw_circle(matchedPairSet[i].keyPointB.col, matchedPairSet[i].keyPointB.row, 3, yellow, 1.0f);
	}
	srcImgWithKpA.display("srcImgWithKpA");
	srcImgWithKpA.save(_saveAddrA);
	srcImgWithKpB.display("srcImgWithKpB");
	srcImgWithKpB.save(_saveAddrB);

}

void Matching::mixImage(const char* mixImgAddr, const char* mixImgWithLineAddr) {
	mixImg = CImg<int>(imgA._width + imgB._width, MAX(imgA._height, imgB._height), 1, 3, 0);
	cimg_forXY(mixImg, x, y) {
		if (x < imgA._width) {
			if (y < imgA._height) {
				mixImg(x, y, 0, 0) = srcImgWithKpA(x, y, 0, 0);
				mixImg(x, y, 0, 1) = srcImgWithKpA(x, y, 0, 1);
				mixImg(x, y, 0, 2) = srcImgWithKpA(x, y, 0, 2);
			}
			else {
				mixImg(x, y, 0, 0) = 0;
				mixImg(x, y, 0, 1) = 0;
				mixImg(x, y, 0, 2) = 0;
			}
		}
		else {
			if (y < imgB._height) {
				mixImg(x, y, 0, 0) = srcImgWithKpB(x - imgA._width, y, 0, 0);
				mixImg(x, y, 0, 1) = srcImgWithKpB(x - imgA._width, y, 0, 1);
				mixImg(x, y, 0, 2) = srcImgWithKpB(x - imgA._width, y, 0, 2);
			}
			else {
				mixImg(x, y, 0, 0) = 0;
				mixImg(x, y, 0, 1) = 0;
				mixImg(x, y, 0, 2) = 0;
			}
		}
	}
	mixImg.display("mixImg");
	mixImg.save(mixImgAddr);

	const double blue[] = { 0, 255, 255 };
	for (int i = 0; i < matchedPairSet.size(); i++) {
		int xa = matchedPairSet[i].keyPointA.col;
		int ya = matchedPairSet[i].keyPointA.row;

		int xb = matchedPairSet[i].keyPointB.col + imgA._width;
		int yb = matchedPairSet[i].keyPointB.row;

		mixImg.draw_line(xa, ya, xb, yb, blue);
	}
	mixImg.display("mixImgWithLine");
	mixImg.save(mixImgWithLineAddr);
}


void Matching::myRANSACtoFindKpTransAndDrawOut(const char* _filename) {

	srand(time(NULL));
	int n = 1000;
	int i = 0;
	int size = matchedPairSet.size();
	int max = 0;
	CvMat *warp_matrix = cvCreateMat(3, 3, CV_32FC1);
	cv::Mat T(3, 3, CV_32FC1);
	while (i < n) {
		
		CvPoint2D32f src[4], dst[4];
		for (int j = 0; j < 4; ++j) {
			int index = rand() % size;
			src[j] = CvPoint2D32f(matchedPairSet[index].keyPointA.row, matchedPairSet[index].keyPointA.col);
			dst[j]= CvPoint2D32f(matchedPairSet[index].keyPointB.row, matchedPairSet[index].keyPointB.col + imgA._width);
		}
		cvGetPerspectiveTransform(src, dst, warp_matrix);
		//warp_matrix = lsq_homog(src, dst, 4);
		cv::Mat E(3, 3, CV_32FC1, warp_matrix->data.fl);
		int num = 0;
		for (auto c : matchedPairSet) {
			cv::Mat A=cv::Mat::ones(3, 1, CV_32FC1);
			A.at<float>(0, 0) = c.keyPointA.row;
			A.at<float>(1, 0) = c.keyPointA.col;
			cv::Mat B = E * A;
			B.at<float>(0, 0) /= B.at<float>(2, 0);
			B.at<float>(1, 0) /= B.at<float>(2, 0);

			double d = sqrt(pow(c.keyPointB.row - B.at<float>(0, 0), 2) + pow(c.keyPointB.col + imgA._width - B.at<float>(1, 0), 2));

			if (d < 80) {
				num++;
			}
		}
		cout << num << endl;
		if (num > max) {
			T = E;
			max = num;
		}
		++i;
	}
	cout << max << endl;
	drawRealKeypoint(_filename, T);
}


void Matching::drawRealKeypoint(const char* _filename, cv::Mat T) {
	//在新的合并图上，画出属于该匹配关系的匹配点pair
	fixedMatchedImg = CImg<int>(imgA._width + imgB._width, imgA._height, 1, 3, 0);
	cimg_forXY(fixedMatchedImg, x, y) {
		if (x < imgA._width) {
			if (y < imgA._height) {
				fixedMatchedImg(x, y, 0, 0) = srcImgWithKpA(x, y, 0, 0);
				fixedMatchedImg(x, y, 0, 1) = srcImgWithKpA(x, y, 0, 1);
				fixedMatchedImg(x, y, 0, 2) = srcImgWithKpA(x, y, 0, 2);
			}
			else {
				fixedMatchedImg(x, y, 0, 0) = 0;
				fixedMatchedImg(x, y, 0, 1) = 0;
				fixedMatchedImg(x, y, 0, 2) = 0;
			}
		}
		else {
			if (y < imgB._height) {
				fixedMatchedImg(x, y, 0, 0) = srcImgWithKpB(x - imgA._width, y, 0, 0);
				fixedMatchedImg(x, y, 0, 1) = srcImgWithKpB(x - imgA._width, y, 0, 1);
				fixedMatchedImg(x, y, 0, 2) = srcImgWithKpB(x - imgA._width, y, 0, 2);
			}
			else {
				fixedMatchedImg(x, y, 0, 0) = 0;
				fixedMatchedImg(x, y, 0, 1) = 0;
				fixedMatchedImg(x, y, 0, 2) = 0;
			}
		}
	}

	const double blue[] = { 0, 255, 255 };
	for (int j = 0; j < matchedPairSet.size(); j++) {    //计算所有匹配向量与v的距离d
		int txa = matchedPairSet[j].keyPointA.col;
		int tya = matchedPairSet[j].keyPointA.row;

		int txb = matchedPairSet[j].keyPointB.col + imgA._width;
		int tyb = matchedPairSet[j].keyPointB.row;

		int tdeltaX = txb - txa;
		int tdeltaY = tyb - tya;

		cv::Mat A = cv::Mat::ones(3, 1, CV_32FC1);
		A.at<float>(0, 0) = tya;
		A.at<float>(1, 0) = txa;
		cv::Mat B = T * A;
		B.at<float>(0, 0) /= B.at<float>(2, 0);
		B.at<float>(1, 0) /= B.at<float>(2, 0);
		//cout << c.keyPointB.row<< " "<<c.keyPointB.col << endl;
		double d = sqrt(pow(tyb - B.at<float>(0, 0), 2) + pow(txb - B.at<float>(1, 0), 2));

		/*auto a = CvPoint2D32f(tya, txa);
		auto b = CvPoint2D32f(tyb, txb);
		auto tmp = persp_xform_pt(a, T);
		auto d = homog_xfer_err(a, tmp, T);*/


		if (d < 80) {    //距离d小于阈值，则视为正确的匹配点
			fixedMatchedImg.draw_line(txa, tya, txb, tyb, blue);
		}
	}

	fixedMatchedImg.display("mixImgWithLine_fixed");
	fixedMatchedImg.save(_filename);
}

Point Matching::getMatchVec() {
	return matchVec;
}
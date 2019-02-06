#ifndef MYMATCHING_H
#define MYMATCHING_H

#include "Sift.h"
#include "CImg.h"
#include <vector>
using namespace cimg_library;

#define FeatureDescGap 1.0
#define InliersGap 500.0

struct Point {
	int col;    //x
	int row;    //y
	Point() : col(-1), row(-1) {}
	Point(int _col, int _row) : col(_col), row(_row) {}
};

struct MatchedPair {
	Point keyPointA;
	Point keyPointB;
	float minDis;
	MatchedPair(Point _pa, Point _pb, float _minDis) : keyPointA(_pa), keyPointB(_pb), minDis(_minDis) {}
};

class Matching
{
public:
	Matching();
	~Matching();
	Matching(int _kp_count_A, Keypoint _firstKeyDesc_A, int _kp_count_B, Keypoint _firstKeyDesc_B);

	/* 特征匹配主函数，得到匹配点pair集 matchedPairSet */
	void featureMatching();

	/* 在原图上画出当前得到的匹配点（不全是真正的匹配点） */
	void drawKeyPoint(const char* _filenameA, const char* _filenameB, const char* _saveAddrA, const char* _saveAddrB);
	
	/* 将两张图片拼在同一张图片上，同时画出匹配点之间连线 */
	void mixImage(const char* mixImgAddr, const char* mixImgWithLineAddr);

	/* 使用RANSAC算法找到真正的匹配点，并画出来 */
	void myRANSACtoFindKpTransAndDrawOut(const char* _filename);
	void drawRealKeypoint(const char* _filename, cv::Mat T);

	Point getMatchVec();

private:
	int keypoint_count_A, keypoint_count_B;
	Keypoint firstKeyDescriptor_A, firstKeyDescriptor_B;

	vector<MatchedPair> matchedPairSet;
	Point matchVec;

	CImg<int> imgA, imgB;
	CImg<int> srcImgWithKpA, srcImgWithKpB;
	CImg<int> mixImg;
	CImg<int> fixedMatchedImg;
};

#endif


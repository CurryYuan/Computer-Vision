#include "Matching.h"
#include "Blending.h"

int main() {
	const char* inputAddr1 = "Output/1-1/blend.bmp";
	const char* inputAddr2 = "Output/1-2/blend.bmp";

	Sift mySift1(inputAddr1, 1);
	mySift1.SiftMainProcess();
	//mySift1.saveImgWithKeypoint("Output/2/1_kp.bmp");

	Sift mySift2(inputAddr2, 1);
	mySift2.SiftMainProcess();
	//mySift2.saveImgWithKeypoint("Output/2/2_kp.bmp");

	Matching myMatching(mySift1.getKeyPointsCount(), mySift1.getFirstKeyDescriptors(),
		mySift2.getKeyPointsCount(), mySift2.getFirstKeyDescriptors());
	myMatching.featureMatching();
	myMatching.drawKeyPoint(inputAddr1, inputAddr2, "Output/1-3/1.bmp", "Output/1-3/2.bmp");
	myMatching.mixImage("Output/1-3/mix.bmp", "Output/1-3/line.bmp");
	myMatching.myRANSACtoFindKpTransAndDrawOut("Output/1-3/fixed.bmp");

	Blending myBlending(myMatching.getMatchVec().col, myMatching.getMatchVec().row);
	myBlending.blendingMainProcess(inputAddr1, inputAddr2);
	myBlending.saveBlendedImg("Output/1-3/blend.bmp");

	return 0;
}

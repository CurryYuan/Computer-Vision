#include "MeanShift.h"

//---------------- Definition ---------------------------------------
#define MS_MAX_NUM_CONVERGENCE_STEPS	5											// up to 5 steps are for convergence
#define MS_MEAN_SHIFT_TOL_COLOR			0.3											// minimum mean color shift change
#define MS_MEAN_SHIFT_TOL_SPATIAL		0.3											// minimum mean spatial shift change
const int dxdy[][2] = { {-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1} };	// Region Growing

// Constructor
Point5D::Point5D() {
	x = -1;
	y = -1;
}

// Set point value
Point5D::Point5D(float px, float py, float pl, float pa, float pb) {
	x = px;
	y = py;
	l = pl;
	a = pa;
	b = pb;
}

// Scale the Lab color to Lab range
void Point5D::PointLab() {
	l = l * 100 / 255;
	a = a - 128;
	b = b - 128;
}

// Sclae the Lab color to range that can be used to transform to RGB
void Point5D::PointRGB() {
	l = l * 255 / 100;
	a = a + 128;
	b = b + 128;
}

// Accumulate points
void Point5D::MSPoint5DAccum(Point5D Pt) {
	x += Pt.x;
	y += Pt.y;
	l += Pt.l;
	a += Pt.a;
	b += Pt.b;
}

// Compute color space distance between two points
float Point5D::MSPoint5DColorDistance(Point5D Pt) {
	return sqrt((l - Pt.l) * (l - Pt.l) + (a - Pt.a) * (a - Pt.a) + (b - Pt.b) * (b - Pt.b));
}

// Compute spatial space distance between two points
float Point5D::MSPoint5DSpatialDistance(Point5D Pt) {
	return sqrt((x - Pt.x) * (x - Pt.x) + (y - Pt.y) * (y - Pt.y));
}

// Scale point
void Point5D::MSPoint5DScale(float scale) {
	x *= scale;
	y *= scale;
	l *= scale;
	a *= scale;
	b *= scale;
}

// Print 5D point
void Point5D::Print() {
	cout << x << " " << y << " " << l << " " << a << " " << b << endl;
}

// Constructor for spatial bandwidth and color bandwidth
MeanShift::MeanShift(float s, float r) {
	hs = s;
	hr = r;
}

void MeanShift::MSSegmentation(CImg<>& Img) {

	//---------------- Mean Shift Filtering -----------------------------
	int ROWS = Img.height();		// Get row number
	int COLS = Img.width();			// Get column number

	Point5D PtCur;					// Current point
	Point5D PtPrev;					// Previous point
	Point5D PtSum;					// Sum vector of the shift vector
	Point5D Pt;
	int Left;						// Left boundary
	int Right;						// Right boundary
	int Top;						// Top boundary
	int Bottom;						// Bottom boundary
	int NumPts;						// number of points in a hypersphere
	int step;

	cimg_forXY(Img, i, j) {
		Left = (i - hs) > 0 ? (i - hs) : 0;						// Get Left boundary of the filter
		Right = (i + hs) < COLS ? (i + hs) : COLS;				// Get Right boundary of the filter
		Top = (j - hs) > 0 ? (j - hs) : 0;						// Get Top boundary of the filter
		Bottom = (j + hs) < ROWS ? (j + hs) : ROWS;				// Get Bottom boundary of the filter
		// Set current point and scale it to Lab color range
		PtCur = Point5D(i, j, (float)Img(i, j, 0), (float)Img(i, j, 1), (float)Img(i, j, 2));
		PtCur.PointLab();
		step = 0;												// count the times
		do {
			PtPrev = PtCur;										// Set the original point and previous one
			PtSum = Point5D(0, 0, 0, 0, 0);						// Initial Sum vector
			NumPts = 0;											// Count number of points that satisfy the bandwidths
			for (int hx = Left; hx < Right; hx++) {
				for (int hy = Top; hy < Bottom; hy++) {
					// Set point in the spatial bandwidth
					Pt = Point5D(hx, hy, (float)Img(hx, hy, 0), (float)Img(hx, hy, 1), (float)Img(hx, hy, 2));
					Pt.PointLab();

					// Check it satisfied color bandwidth or not
					if (Pt.MSPoint5DColorDistance(PtCur) < hr) {
						PtSum.MSPoint5DAccum(Pt);				// Accumulate the point to Sum vector
						NumPts++;								// Count
					}
				}
			}
			PtSum.MSPoint5DScale(1.0 / NumPts);					// Scale Sum vector to average vector
			PtCur = PtSum;										// Get new origin point
			step++;												// One time end
		// filter iteration to end
		} while ((PtCur.MSPoint5DColorDistance(PtPrev) > MS_MEAN_SHIFT_TOL_COLOR) &&
			(PtCur.MSPoint5DSpatialDistance(PtPrev) > MS_MEAN_SHIFT_TOL_SPATIAL) && (step < MS_MAX_NUM_CONVERGENCE_STEPS));

		// Scale the color
		PtCur.PointRGB();
		// Copy the result to image
		Img(i, j, 0) = PtCur.l;
		Img(i, j, 1) = PtCur.a;
		Img(i, j, 2) = PtCur.b;
	}
	//--------------------------------------------------------------------

	//----------------------- Segmentation ------------------------------
	int RegionNumber = 0;			// Reigon number
	int label = -1;					// Label number
	float *Mode = new float[ROWS * COLS * 3];					// Store the Lab color of each region
	int *MemberModeCount = new int[ROWS * COLS];				// Store the number of each region
	memset(MemberModeCount, 0, ROWS * COLS * sizeof(int));		// Initialize the MemberModeCount

	// Label for each point
	int **Labels = new int *[ROWS];
	for (int i = 0; i < ROWS; i++)
		Labels[i] = new int[COLS];

	// Initialization
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			Labels[i][j] = -1;
		}
	}

	cimg_forXY(Img, i, j) {
		// If the point is not being labeled
		if (Labels[i][j] < 0) {
			Labels[i][j] = ++label;		// Give it a new label number
			// Get the point
			PtCur = Point5D(i, j, (float)Img(i, j, 0), (float)Img(i, j, 1), (float)Img(i, j, 2));
			PtCur.PointLab();

			// Store each value of Lab
			Mode[label * 3 + 0] = PtCur.l;
			Mode[label * 3 + 1] = PtCur.a;
			Mode[label * 3 + 2] = PtCur.b;

			// Region Growing 8 Neighbours
			vector<Point5D> NeighbourPoints;
			NeighbourPoints.push_back(PtCur);
			while (!NeighbourPoints.empty()) {
				Pt = NeighbourPoints.back();
				NeighbourPoints.pop_back();

				// Get 8 neighbours
				for (int k = 0; k < 8; k++) {
					int hx = Pt.x + dxdy[k][0];
					int hy = Pt.y + dxdy[k][1];
					if ((hx > 0) && (hy > 0) && (hx < ROWS) && (hy < COLS) && (Labels[hx][hy] < 0)) {
						Point5D P(hx, hy, (float)Img(hx, hy, 0), (float)Img(hx, hy, 1), (float)Img(hx, hy, 2));
						P.PointLab();

						// Check the color
						if (PtCur.MSPoint5DColorDistance(P) < hr) {
							// Satisfied the color bandwidth
							Labels[hx][hy] = label;				// Give the same label					
							NeighbourPoints.push_back(P);		// Push it into stack
							MemberModeCount[label]++;			// This region number plus one
							// Sum all color in same region
							Mode[label * 3 + 0] += P.l;
							Mode[label * 3 + 1] += P.a;
							Mode[label * 3 + 2] += P.b;
						}
					}
				}
			}
			MemberModeCount[label]++;							// Count the point itself
			Mode[label * 3 + 0] /= MemberModeCount[label];		// Get average color
			Mode[label * 3 + 1] /= MemberModeCount[label];
			Mode[label * 3 + 2] /= MemberModeCount[label];
		}
	}
	RegionNumber = label + 1;										// Get region number

	// Get result image from Mode array
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			label = Labels[i][j];
			float l = Mode[label * 3 + 0];
			float a = Mode[label * 3 + 1];
			float b = Mode[label * 3 + 2];
			Point5D Pixel(i, j, l, a, b);
			Pixel.PointRGB();
			// Pixel.Print();
			Img(i, j, 0) = Pixel.l;
			Img(i, j, 1) = Pixel.a;
			Img(i, j, 2) = Pixel.b;
		}
	}
	//--------------------------------------------------------------------

	//	for(int i = 0; i < ROWS; i++){
	//		for(int j = 0; j < COLS - 1; j++){
	//			if(Labels[i][j] != Labels[i][j + 1])
	//				Img.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
	//		}
	//	}

	//--------------- Delete Memory Applied Before -----------------------
	/*delete[] Mode;
	delete[] MemberModeCount;

	for (int i = 0; i < ROWS; i++)
		delete[] Labels[i];
	delete[] Labels;*/
}


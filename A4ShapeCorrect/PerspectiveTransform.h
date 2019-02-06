#pragma once

class PerspectiveTransform {
public:
	float a11, a12, a13, a21, a22, a23, a31, a32, a33;

public:
	PerspectiveTransform(float, float, float, float, float, float, float, float, float);
	PerspectiveTransform();

	PerspectiveTransform quadrilateralToQuadrilateral(float, float, float, float, float,
		float, float, float, float, float, float, float, float, float, float, float);

	PerspectiveTransform squareToQuadrilateral(float, float, float, float, float, float, float, float);

	PerspectiveTransform quadrilateralToSquare(float, float, float, float, float, float, float, float);

	PerspectiveTransform buildAdjoint();

	PerspectiveTransform times(PerspectiveTransform);
};
#ifndef MYVECTOR_H
#define MYVECTOR_H

class MyVector
{
public:
   MyVector();
	MyVector(float x0, float y0, float z0, float x1, float y1, float z1);
	float calcMag();
   float calcDot(MyVector &vect);
	void normalize();
   void calcCross(MyVector &vect);
   float endX;
	float endY;
	float endZ;
	float startX;
	float startY;
	float startZ;
	float normX;
	float normY;
	float normZ;
	float crossX;
	float crossY;
	float crossZ;
	float mag;
};
#endif

#include "MyVector.h"
#include <math.h>

MyVector::MyVector()
{
   startX = 0;
   startY = 0;
   startZ = 0;
   endX = 0;
   endY = 0;
   endZ = 0;
   normX = 0;
   normY = 0;
   normZ = 0;
   crossX = 0;
   crossY = 0;
   crossZ = 0;
   mag = 0;
}

MyVector::MyVector(float x0, float y0, float z0, float x1, float y1, float z1):
   startX(x0), startY(y0), startZ(z0), endX(x1), endY(y1), endZ(z1)
{
   mag = calcMag();
}

float MyVector::calcMag()
{
	return sqrt(pow((MyVector::startX - MyVector::endX),2) + pow((MyVector::startY - MyVector::endY),2) + pow((MyVector::startZ - MyVector::endZ),2));
}

float MyVector::calcDot(MyVector &vect){
   return MyVector::normX * vect.normX + MyVector::normY * vect.normY + MyVector::normZ * vect.normZ;
}

void MyVector::calcCross(MyVector &vect){
   MyVector::crossX = MyVector::endY * vect.endZ - MyVector::endZ * vect.endY;
   MyVector::crossY = MyVector::endZ * vect.endX - MyVector::endX * vect.endZ;
   MyVector::crossZ = MyVector::endX * vect.endY - MyVector::endY * vect.endX;
}

void MyVector::normalize()
{
    MyVector::normX = MyVector::endX/MyVector::mag;
    MyVector::normY = MyVector::endY/MyVector::mag;
    MyVector::normZ = MyVector::endZ/MyVector::mag;
}

/*MyVector MyVector::nomalizeVect()
{
	return null;
}*/

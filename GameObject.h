#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include "MyVector.h"
#include <vector>
using namespace std;

class GameObject
{
public:
   typedef struct BoundingBox{
         float lowerLeftX;
         float lowerLeftY;
         float lowerLeftZ;
         float upperRightX;
         float upperRightY;
         float upperRightZ;
      } BoundingBox;

   GameObject(void);
   GameObject(MyVector _position, MyVector _direction, float velocity);//, BoundingBox _boundingBox);
   int Collision(BoundingBox colliding);
   void setbounds(float boundX, float boundZ);
   ~GameObject(void);
   void step(float dt);
   int collidingWithObjects(vector<GameObject> gameObjects);
   
   MyVector position;
   MyVector direction;   
   float velocity; 
   BoundingBox boundingBox;
   
   float boundsX;
   float boundsZ;
};

#endif

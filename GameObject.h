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
   GameObject(int _id, MyVector _position, MyVector _direction, float velocity);//, BoundingBox _boundingBox);
   int Collision(MyVector pos, BoundingBox colliding);
   void setbounds(float boundX, float boundZ);
   void setBoundingBox(float llX, float llY, float llZ, float urX, float urY, float urZ);
   ~GameObject(void);
   void step(float dt, std::vector<GameObject>* gameObjects);
   int collidingWithObjects(vector<GameObject> gameObjects);
   
   MyVector position;
   MyVector direction;   
   float velocity; 
   BoundingBox boundingBox;
   
   
   float boundsX;
   float boundsZ;
   
   float R, G, B;
   bool alive;
   
   int id;
};

#endif

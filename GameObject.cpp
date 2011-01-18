#include "GameObject.h"
#include <math.h>
#include <iostream>

GameObject::GameObject(void)
{
}

GameObject::GameObject(MyVector _position, MyVector _direction, float _velocity)//, BoundingBox _boundingBox)
{
   position.endX = _position.endX;
   position.endY = _position.endY;
   position.endZ = _position.endZ;
   
   direction.endX = _direction.endX;
   direction.endY = _direction.endY;
   direction.endZ = _direction.endZ;
   
   velocity = _velocity;
   boundingBox.lowerLeftX = -.1;//_boundingBox.lowerLeftX;
   boundingBox.lowerLeftY = 0;//_boundingBox.lowerLeftY;
   boundingBox.lowerLeftZ = -.1;//_boundingBox.lowerLeftZ;
   boundingBox.upperRightX = .1;//_boundingBox.upperRightX;
   boundingBox.upperRightY = .2;//_boundingBox.upperRightY;
   boundingBox.upperRightZ = .1;//_boundingBox.upperRightZ;   
   boundsX = 4;
   boundsZ = 4;
}

GameObject::~GameObject(void)
{
}

void GameObject::setBoundingBox(float llX, float llY, float llZ, float urX, float urY, float urZ)
{
   boundingBox.lowerLeftX = llX;
   boundingBox.lowerLeftY = llY;
   boundingBox.lowerLeftZ = llZ;
   boundingBox.upperRightX = urX;
   boundingBox.upperRightY = urY;
   boundingBox.upperRightZ = urZ;
}

int GameObject::Collision(MyVector pos, BoundingBox colliding)
{
   if((position.endX + boundingBox.lowerLeftX) > (pos.endX + colliding.upperRightX))
      return false;
   if((position.endY + boundingBox.lowerLeftY) > (pos.endY + colliding.upperRightY))
      return false;  
   if((position.endZ + boundingBox.lowerLeftZ) > (pos.endZ + colliding.upperRightZ))
      return false;   
   if((position.endX + boundingBox.upperRightX) < (pos.endX + colliding.lowerLeftX))
      return false;
   if((position.endY + boundingBox.upperRightY) < (pos.endY + colliding.lowerLeftY))
      return false;
   if((position.endZ + boundingBox.upperRightZ) < (pos.endZ + colliding.lowerLeftZ))
      return false;
   return true;
}

void GameObject::setbounds(float boundX, float boundZ)
{
   boundsX = boundX;
   boundsZ = boundZ;
}

// -1 if not colliding
// int if colliding with the index given
int GameObject::collidingWithObjects(vector<GameObject> gameObjects)
{

   for(int i = 0;i < gameObjects.size();i++)
   {
      if(Collision(gameObjects[i].position, gameObjects[i].boundingBox) != 0)
      {
         return i;
      }
   }
   return -1;
}

void GameObject::step(float dt)
{
   /*The step function receives one parameter, dt, the elapsed time. It must update the position by
converting dt into elapsed time in seconds, then using that time value, the velocity and direction,
update the position. However, it must check two constraints:
1. If the new position would be off the grid, negate the direction vector and recompute the new
position.
2. If the new position would cause the object's bounding box to intersect the bounding of any
other object, do not update the position.*/
   float timeInSec = 0;
   MyVector stepAmount = *new MyVector(0,0,0,0,0,0);
   float theta = 0;
   
   timeInSec = dt;
   
   theta = atan2(direction.endZ,direction.endX);
   stepAmount.endX = cos(theta) * (timeInSec * velocity);
   stepAmount.endZ = sin(theta) * (timeInSec * velocity);
   
   if((position.endX + stepAmount.endX) > boundsX || (position.endX + stepAmount.endX) < -boundsX)
   {
      direction.endX = -direction.endX;
      theta = atan2(direction.endZ,direction.endX);
      stepAmount.endX = cos(theta) * (timeInSec * velocity);
      stepAmount.endZ = sin(theta) * (timeInSec * velocity);
   }
   if((position.endZ + stepAmount.endZ) > boundsZ || (position.endZ + stepAmount.endZ) < -boundsZ)
   {
      direction.endZ = -direction.endZ;
      theta = atan2(direction.endZ,direction.endX);
      stepAmount.endX = cos(theta) * (timeInSec * velocity);
      stepAmount.endZ = sin(theta) * (timeInSec * velocity);
   }
   
   position.endX += stepAmount.endX;
   position.endZ += stepAmount.endZ;
}













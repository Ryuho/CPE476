#include "GameObject.h"
#include <math.h>

GameObject::GameObject(void)
{
}

GameObject::GameObject(MyVector _position, MyVector _direction, float _velocity, BoundingBox _boundingBox)
{
   position.endX = _position.endX;
   position.endY = _position.endY;
   position.endZ = _position.endZ;
   
   direction.endX = _direction.endX;
   direction.endY = _direction.endY;
   direction.endZ = _direction.endZ;
   
   velocity = _velocity;
   boundingBox.lowerLeftX = _boundingBox.lowerLeftX;
   boundingBox.lowerLeftY = _boundingBox.lowerLeftY;
   boundingBox.lowerLeftZ = _boundingBox.lowerLeftZ;
   boundingBox.upperRightX = _boundingBox.upperRightX;
   boundingBox.upperRightY = _boundingBox.upperRightY;
   boundingBox.upperRightZ = _boundingBox.upperRightZ;   
}

GameObject::~GameObject(void)
{
}

int GameObject::Collision(BoundingBox colliding)
{
   if(boundingBox.lowerLeftX > colliding.upperRightX)
      return false;
   if(boundingBox.lowerLeftY > colliding.upperRightY) 
      return false;  
   if(boundingBox.lowerLeftZ > colliding.upperRightZ)
      return false;   
   if(boundingBox.upperRightX > colliding.lowerLeftX) 
      return false;
   if(boundingBox.upperRightY < colliding.lowerLeftY)
      return false;
   if(boundingBox.upperRightZ < colliding.lowerLeftZ)
      return false;
   return true;
}

void GameObject::setbounds(float boundX, float boundZ)
{
   boundsX = boundX;
   boundsZ = boundZ;
}

int GameObject::collidingWithObjects(vector<GameObject> gameObjects)
{

   for(int i = 0;i < gameObjects.size();i++)
   {
      if(Collision(gameObjects[i].boundingBox) != 0 && gameObjects[i].position.endX != position.endX && gameObjects[i].position.endZ != position.endZ)
      {
         return 1;
      }
   }
   return 0;
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
   MyVector newPosition = *new MyVector(0,0,0,0,0,0);
   float theta = 0;
   
   timeInSec = dt/60.0;
   
   theta = atan(direction.endZ/direction.endX);// need to convert to radians?
   newPosition.endX = cos(theta) * (timeInSec * velocity);
   newPosition.endZ = sin(theta) * (timeInSec * velocity);
   
   if(newPosition.endX > boundsX || newPosition.endX < -boundsX)
   {
      //negate dir vector and recompute new position
      direction.endX = -direction.endX;
      theta = atan(direction.endZ/direction.endX);// need to convert to radians?
      newPosition.endX = cos(theta) * (timeInSec * velocity);      
   }
   if(newPosition.endZ > boundsZ || newPosition.endZ < -boundsZ)
   {
      direction.endZ = -direction.endZ;
      theta = atan(direction.endZ/direction.endX);// need to convert to radians?
      newPosition.endZ = sin(theta) * (timeInSec * velocity);
   }
   position.endX = newPosition.endX;
   position.endZ = newPosition.endZ;
   
}













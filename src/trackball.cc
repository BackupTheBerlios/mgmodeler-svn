#include "trackball.h"

//Find Z so that point v(mousex,mousey,Z) is on a unit sphere.
//if we are out of the sphere, we find Z so that point v is on a hyperbole.
//The hyperbole allow continuity when we go away from the sphere.
//X=1/sqrt(2) is the x-coordinate of the intersection between a unit circle and
//the hyperbole 1/2x
//Reference: Terence J. Grant nehe.gamedev.net and nvidia's
//trackball.h(Gavin Bell)
void
Trackball::mapToSphere(Vec3f &v) 
{
  float len2;
      
  v.x = (2*v.x)/float(screenWidth-1)-1;
  v.y = 1-(2*v.y)/float(screenHeight-1);
  v.z = 0;
  if((len2 = v.length2())<INVSQRT2)
      v.z = std::sqrt(1.0-len2); // We are on the sphere
  else 
      v.z = 1.0f/(2*std::sqrt(len2)); // On the hyperbole
}

void
Trackball::computeOrientation(int x, int y) 
{
  Quaternionf q;
  Vec3f endVector(x, y, 0);
  
  mapToSphere(endVector);
  q.toRotationArc(startVector, endVector);
  orientation = q*startOrientation;
  orientation.normalize();
}

void
Trackball::startRotation(int x, int y)
{
  startVector.setValues(x, y, 0);
  mapToSphere(startVector);
}

void
Trackball::stopRotation()
{
  startOrientation = orientation;
}

void
Trackball::getRotation(Matrix4f &m)
{
  orientation.unitToMatrix44 (m);
}

const float Trackball::INVSQRT2=1.0f/std::sqrt(2.0f);

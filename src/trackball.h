#ifndef TRACKBALL_H
#define TRACKBALL_H

#include "math/vector3.h"
#include "math/matrix.h"
#include "math/quaternion.h"

class Trackball {
private:
  static const float INVSQRT2;
  
  Quaternionf orientation, startOrientation;
  Vec3f startVector, endVector;
  unsigned int screenWidth, screenHeight;
  
  void mapToSphere(Vec3f &v);

public:
  Trackball(unsigned int screenWidth, unsigned int screenHeight):
    screenWidth(screenWidth), screenHeight(screenHeight)  {
    orientation.toIdentity();
  }

  void setScreenSize(unsigned int screenWidth, unsigned int screenHeight) { 
    this->screenHeight = screenHeight;
    this->screenWidth = screenWidth;
  }
  void startRotation(int x, int y);
  void stopRotation();
  void computeOrientation(int x, int y);
  void getRotation(Matrix4f &m);
};

#endif

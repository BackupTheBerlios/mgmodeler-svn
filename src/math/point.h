#ifndef POINT_H
#define POINT_H

#include <vector>
#include "math/vector3.h"

class Point {
 public:
  Point (const Vec3f& coords = Vec3f(0, 0, 0), 
	 const Vec3f& normal = Vec3f (1, 0, 0),
	 const Vec3f& tangent = Vec3f(0, -1, 0),
	 const Vec3f& color = Vec3f(1, 1, 1), float alpha = 1) : 
    coords(coords), normal(normal), color(color), 
    tangent(tangent), alpha(alpha) 
    {}
  void setCoords(const Vec3f& coords) {
    this->coords=coords;
  }

  void setNormal(const Vec3f& normal) {
    this->normal=normal;
  }

  void setTangent(const Vec3f& tangent) {
    this->tangent=tangent;
  }

  void setColor(const Vec3f& color) {
    this->color=color;
  }
  
  void setAlpha(float  alpha) {
    this->alpha=alpha;
  }
  
  const Vec3f& getCoords() const {
    return coords;
  }

  const Vec3f& getNormal() const {
    return normal;
  }
  
  const Vec3f& getColor() const {
    return color;
  }

  const Vec3f& getTangent() const {
    return tangent;
  }

  Vec3f& getCoords() {
    return coords;
  }

  Vec3f& getNormal() {
    return normal;
  }
  
  Vec3f& getColor() {
    return color;
  }
  
  Vec3f& getTangent() {
    return tangent;
  }
  
  const float getAlpha () const {
      return alpha;
  }

  float& getAlpha () {
    return alpha;
  }

  Point& operator+=(const Vec3f& v) {
    coords+=v;
    return *this;
  }

  Point& operator-=(const Vec3f& v) {
    coords-=v;
    return *this;
  }

  Point& operator*=(const float f) {
    coords*=f;
    return *this;
  }

 private:
  Vec3f coords;
  Vec3f normal;
  Vec3f color;
  Vec3f tangent;

  float alpha;
};

typedef std::vector<Point> Face;

#endif /* ! POINT_H */

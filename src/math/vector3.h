#ifndef VECTOR3_H
#define VECTOR3_H
    
#include <iostream>
#include <cmath>
#include <cassert>
#include <limits>

template<class T>
class Vector3
{
public:
  static const T epsilon;
  T x, y, z;
    
public:
  Vector3(): x(0), y(0), z(0) {}
  Vector3(T inX, T inY, T inZ): x(inX), y(inY), z(inZ) {}
  Vector3(const Vector3<T>& v): x(v.x), y(v.y), z(v.z) {}

  void getValue(T &x, T &y, T &z) const {
    x = this->x; y = this->y; z = this->z;
  }

  void setValues(T x, T y, T z) {
    this->x = x; this->y = y; this->z = z;
  }

  void setValues(const Vector3<T>& v) {
    x = v.x; y = v.y; z = v.z;
  }
  
  void setValues(const T v[3]) {
    x = v[0]; y = v[1]; z = v[2];
  }

  T maxValue() {
    return std::max(std::max(x, y), z);
  }

  T& operator[](unsigned int i) {
    assert(i<3);
    return *(&x+i);
  } 
  
  const T &operator[](unsigned int i) const {
    assert(i<3);
    return *(&x+i);
  }

  Vector3<T> operator-() const {
    return Vector3<T>(-x, -y, -z);
  } 
  
  Vector3<T>& operator=(const Vector3<T> &v) {
    x = v.x; y = v.y; z = v.z;
    return *this;
  }

  Vector3<T>& operator+=(const Vector3<T> &v) {
    x += v.x; y += v.y; z += v.z;
    return *this;
  }
  
  Vector3<T>& operator-=(const Vector3<T>& v)  {
    x -= v.x; y -= v.y; z -= v.z;
    return *this;
  }
  
  Vector3<T>& operator*=(T k) {
    x *= k; y *= k; z *= k;
    return *this;
  }
  
  Vector3<T>& operator/=(T k)  {
    assert(k != 0);
    x /= k; y /= k; z /= k;
    return *this;
  }
  
  Vector3<T> operator+(const Vector3<T> &v) const {
    return Vector3<T>(x+v.x, y+v.y, z+v.z);
  }
  
  Vector3<T> operator-(const Vector3<T> &v) const {
    return Vector3<T>(x-v.x, y-v.y, z-v.z);
  }
  
  Vector3<T> operator^(const Vector3<T> &v) const {
    return cross(v);
  }

  T operator*(const Vector3<T> &v) const {
    return dot(v);
  }
  
  Vector3<T> operator*(T k) const {
    return Vector3<T>(x*k, y*k, z*k);
  }
  
  Vector3<T> operator/(T k) const {
    assert(k!=0);
    return Vector3<T>(x/k, y/k, z/k);
  }
  
  bool operator==(const Vector3<T> &v) const {
    using namespace std;
    return abs(x-v.x)<epsilon && abs(y-v.y)<epsilon && abs(z-v.z)<epsilon;
  }

  bool isNull() const {
    return *this == Vector3<T>(0,0,0);
  }
  
  T dist(const Vector3<T> &v) const {
    return (*this - v).length();
  }
  
  T dist2(const Vector3<T> &v) const {
    return (*this - v).length2();
  }
  
  T length() const {
    return std::sqrt(x*x+y*y+z*z);
  }
  
  T length2() const {
    return x*x+y*y+z*z;
  }
  
  void normalize() {
    T l = length();
    if(l == 0)
      x=y=z=0;
    else
      *this *= T(1.0)/l;
  }
  
  T dot(const Vector3<T> &v) const {
    return x*v.x+y*v.y+z*v.z;
  }
  
  Vector3<T> cross(const Vector3<T> &v) const {
    return Vector3<T>(y*v.z - z*v.y,
                      z*v.x - x*v.z,
                      x*v.y - y*v.x);
  }

  friend Vector3<T> operator*(T k, const Vector3<T>&v) {
    return v*k;
  }

  friend std::ostream& operator<<(std::ostream& os, const Vector3<T>& v) {
      return os << v.x << ", " << v.y << ", " << v.z;
  }

};

template<class T>
const T Vector3<T>::epsilon = std::numeric_limits<T>::epsilon();

typedef Vector3<float> Vec3f;

#endif

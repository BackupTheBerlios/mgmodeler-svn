#ifndef MATRIX4_H
#define MATRIX4_H

#include <iostream>
#include <string>
#include <cassert>

template<class T>
class Matrix4{
 public:
  Matrix4(){}
  Matrix4( T x ){ 
    coeff[0][0] = x; coeff[0][1] = x; coeff[0][2] = x; coeff[0][3] = x;
    coeff[1][0] = x; coeff[1][1] = x; coeff[1][2] = x; coeff[1][3] = x;
    coeff[2][0] = x; coeff[2][1] = x; coeff[2][2] = x; coeff[2][3] = x;
    coeff[3][0] = x; coeff[3][1] = x; coeff[3][2] = x; coeff[3][3] = x;
  }

  void setValues(T m00, T m01, T m02, T m03,
                 T m10, T m11, T m12, T m13,
                 T m20, T m21, T m22, T m23,
                 T m30, T m31, T m32, T m33) {
    coeff[0][0] = m00; coeff[0][1] = m01; coeff[0][2] = m02; coeff[0][3] = m03;
    coeff[1][0] = m10; coeff[1][1] = m11; coeff[1][2] = m12; coeff[1][3] = m13;
    coeff[2][0] = m20; coeff[2][1] = m21; coeff[2][2] = m22; coeff[2][3] = m23;
    coeff[3][0] = m30; coeff[3][1] = m31; coeff[3][2] = m32; coeff[3][3] = m33;
  }
  
  inline void Zero() {
    memset( coeff, 0, 16*sizeof( T ) );
  }

  inline void Identity() {
    Zero();
    coeff[0][0] = 1.0; coeff[1][1] = 1.0; coeff[2][2] = 1.0; coeff[3][3] = 1.0;
  }

  T * operator[] (unsigned int i) {
    assert(i<4);
    return coeff[i];
  }

  bool operator==(const Matrix4<T> &m) const {
    T epsilon = std::numeric_limits<T>::epsilon();
    for( int j = 0; j < 4; j++)
      for( int i = 0; i < 4; i++ )
        if( std::abs(coeff[i][j]-m.coeff[i][j]) > epsilon )
          return false;
    return true;
  }

  bool isIdentity() {
    Matrix4<T> m;
    m.Identity();
    return m == *this;    
  }

  friend std::ostream& operator<<(std::ostream& os, const Matrix4<T>& m) {
    using namespace std;
    for( int i = 0; i < 4; i++ ) {
      os << "[ ";
      for( int j = 0; j < 4; j++ )
        os << m.coeff[i][j] << " ";
      os << "]" << std::endl;
    }
    return os;
  }

private:
  T coeff[4][4];
};

#endif

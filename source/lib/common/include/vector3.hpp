#ifndef __VECTOR3_H__
#define __VECTOR3_H__
// #------------------------------#
// |          includes            |
// #------------------------------#
// c/c++ includes
#include <stdint.h>
#include <cmath>

// my includes

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

template<typename T = int16_t>
struct Vector3
{
  union{
    struct
    {
      T x;
      T y;
      T z;
    };
    T arr[3];
  };

  void normalize()
  {
    const int scale = 100;
    auto vector_length = std::sqrt(x*x + y*y + z*z);
    x = (100.0f * (float)x) / vector_length;
    y = (100.0f * (float)y) / vector_length;
    z = (100.0f * (float)z) / vector_length;
  }
  Vector3<T> operator- (const Vector3<T>& other)
  {
    T x = this->x - other.x;
    T y = this->y - other.y;
    T z = this->z - other.z;
    return Vector3<T>{x,y,z};
  }
  Vector3<T> operator+ (const Vector3<T>& other)
  {
    T x = this->x + other.x;
    T y = this->y + other.y;
    T z = this->z + other.z;
    return Vector3<T>{x,y,z};
  }
  template<typename Q>
  Vector3<T> operator* (const Q constant)
  {
    T x = this->x * constant;
    T y = this->y * constant;
    T z = this->z * constant;
    return Vector3<T>{x,y,z};
  }
  template<typename Q>
  Vector3<T> operator/ (const Q constant)
  {
    T x = this->x / constant;
    T y = this->y / constant;
    T z = this->z / constant;
    return Vector3<T>{x,y,z};
  }
  template<typename Q>
  explicit operator Vector3<Q>() {
    return Vector3<Q>{(Q)x, (Q)y, (Q)z};
  }
};

// #------------------------------#
// | static variables definitions |
// #------------------------------#

// #------------------------------#
// | static functions declarations|
// #------------------------------#

// #------------------------------#
// | global function definitions  |
// #------------------------------#

// #------------------------------#
// | static functions definitions |
// #------------------------------#

#endif

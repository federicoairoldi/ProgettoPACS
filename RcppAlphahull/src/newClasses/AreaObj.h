#ifndef _AREAOBJ_H_
#define _AREAOBJ_H_

#include "../MyGAL/Vector2.h"
using namespace mygal;

template<typename T>
class AreaObj{
  typedef Vector2<T> vector2;
  
  public:
    // Returns if the given point belongs to the ball or not
    virtual bool isIn(const vector2& p) const  = 0;
    virtual bool isIn(const T& xp, const T& yp) const = 0;
    // Returns if the given point is on the boundary of the ball or not
    virtual bool isOnBound(const vector2& p) const = 0;
    virtual bool isOnBound(const T& xp, const T& yp) const  = 0;
    // Returns the area of the object
    virtual T area() const = 0;
};

#endif
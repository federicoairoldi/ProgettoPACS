#include "../MyGAL/Vector2.h"
#include "Rect.h"
#include <limits>
#include <ostream>
using namespace mygal;

#ifndef _SEGMENT_
#define _SEGMENT_

template<typename T> class Segment;

// Inserts a segment in a stream
template<typename T>
std::ostream& operator<<(std::ostream& os, const Segment<T>& seg){
  os << seg.p << "<->" << seg.q;
  return os;
}

// class to define open segments in R2
template<typename T>
class Segment{
  typedef Vector2<T> vector;
  typedef Rect<T> rect;
  
  // FRIENDS
  friend std::ostream& operator<<<T>(std::ostream& os, const Segment<T>& seg);

  private:
    // ATTRIBUTES
    vector p, q;

  public:
    // CONSTRUCTORS
    Segment() = delete;
    Segment(const vector& p, const vector& q): p(p), q(q) {}
    Segment(const T& xp, const T& yp, const T& xq, const T& yq): p(xp,yp), q(xq,yq) {}

    // GETTERS
    // Returns the slope of the corresponding rect for the segment: I use as convention a +inf slope for
    // rects of the form x = const
    T slope() const { return (isVertical()? std::numeric_limits<T>::infinity(): (q.y-p.y)/(q.x-p.x)); }
    // Returns the intercept of the corresponding rect for the segment: I return the x coordinate of the
    // points in the case that the segment is vertical
    T intercept() const { return (isVertical()? -std::numeric_limits<T>::infinity(): q.y-q.x*(q.y-p.y)/(q.x-p.x)); }
    // Returns the direction rect for the segment
    rect getRect() const{ return rect(p,q); }
    // Returns true if the rect corresponding to the segment has form x = const
    bool isVertical() const {return p.x == q.x;}
    
    // OTHER METHODS
    // Returns wheter or not two segment intersect (in R2)
    bool intersect(const Segment<T>& other) const{
      // retrieving rects
      rect r1=getRect(), r2=other.getRect();
      
      // check position of 2nd segment w.r.t. the 1st: if val1 and val2 have different signs then the segment s2 crosses the rect r1
      int val1 = r1.eval(other.p);
      int val2 = r1.eval(other.q);
      // check position of 1st segment w.r.t. the 2nd: if val3 and val4 have different signs then the segment s1 crosses the rect r2
      int val3 = r2.eval(p);
      int val4 = r2.eval(q);
      
      
      if(val1!=val2 && val3!=val4)
        return true;

      // colinear case
      if( val1 == 0 && val2 == 0 )
        return ( inRange(other.p) || inRange(other.q) );

      return false;
    }
    
    /* Given a point and a segment which both lie on the same rect, check whether or not the
     * point falls inside the segment.
     */
    bool inRange(const Vector2<T>& point) const{
      T inf=std::min(p.x, q.x), sup=std::max(p.x, q.x);
      if( point.x <= inf || sup <= point.x )
        return false;
      
      inf=std::min(p.y, q.y); sup=std::max(p.y, q.y);
      if( point.y <= inf || sup <= point.y )
        return false;
      
      return true;
    }
    
    /* Returns whether or not the given point falls in the segment
     * first evaluate whether or not the point is on the rect
     * secondly check that the point coordinates fall in the same range of the segment span
     * if both condition are true then the point is in the segment
     */
    bool inside(const Vector2<T>& point) const { return getRect().eval(point)==0 && inRange(point); };
};

template<typename T>
bool intersect(const Segment<T>& s1, const Segment<T>& s2){ return s1.intersect(s2); }

template<typename T>
bool inside(const Segment<T>& seg, const Vector2<T>& point) { return seg.inside(point); }

template<typename T>
bool inRange(const Segment<T>& seg, const Vector2<T>& point){ return seg.inRange(point); }

#endif

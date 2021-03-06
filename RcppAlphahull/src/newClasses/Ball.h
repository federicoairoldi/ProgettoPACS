#ifndef _BALL_
#define _BALL_

#include <cmath>
#include <ostream>
#include "geomUtil.h"
#include "AreaObj.h"
#include "CircArc.h"
#include "../MyGAL/Vector2.h"
using namespace mygal;

template<typename T> class Ball;
template<typename T> class Arc;

// Inserts a ball in a stream
template<typename T>
std::ostream& operator<<(std::ostream& os, const Ball<T>& ball){
  os << "center: " << ball.c << " radius: " << ball.r;
  return os;
} 

// Returns true if the two balls are equal
template<typename T>
bool operator==(const Ball<T>& b1, const Ball<T>& b2){ return ( b1.c == b2.c && b1.r == b2.r ); }

// Returns true if the two balls are different
template<typename T>
bool operator!=(const Ball<T>& b1, const Ball<T>& b2) { return !(b1==b2); };

// Computes intersection points between two balls with different centers
template<typename T>
std::vector<Vector2<T>> intersections(const Ball<T>& b1, const Ball<T>& b2){ return b1.intersections(b2); }

// Computes the intersection arc (on the caller circumference) between two balls with different centers
template<typename T>
CircArc<T> intersection_arc(const Ball<T>& b1, const Ball<T>& b2){ return b1.intersection_arc(b2); };

template<typename T>
class Ball: public AreaObj<T>{
  // FRIENDS
  friend std::ostream& operator<<<T>(std::ostream& os, const Ball<T>& ball);
  friend bool operator==<T>(const Ball<T>& b1, const Ball<T>& b2);
  
  typedef Vector2<T> vector;
  typedef CircArc<T> arc;
  
  private:
    // ATTRIBUTES
    vector c; // center of the ball
    T r; // radius of the ball

  public:
    // CONSTRUCTORS
    Ball() = delete;
    Ball(const vector& c, const T& r): c(c), r(r) {};
    Ball(const T& xc, const T& yc, const T& r): c(xc,yc), r(r) {};

    // GETTERS
    vector center() const { return c; };
    T radius() const { return r; };
    T perimeter() const { return 2*M_PI*r; };
    // Returns the area of the object
    T area() const override { return M_PI*r*r; };
    
    // OTHER METHODS
    // Returns if the given point belongs to the ball or not
    bool isIn(const vector& p) const override { return isIn(p.x,p.y); };
    bool isIn(const T& xp, const T& yp) const override { return ((xp-c.x)*(xp-c.x)+(yp-c.y)*(yp-c.y)) < r*r; }
    // Returns if the given point is on the boundary of the ball or not
    bool isOnBound(const vector& p) const override { return isOnBound(p.x,p.y); };
    bool isOnBound(const T& xp, const T& yp) const override { return ((xp-c.x)*(xp-c.x)+(yp-c.y)*(yp-c.y)) == r*r; };
    // Checks if the balls share same center
    bool concentric(const Ball<T>& b) const { return c==b.c; }; 
    // Checks if b2 falls entirely in the ball
    bool containsBall(const Ball<T> b) const { return c.getDistance(b.c) <= std::fabs(r-b.r) && r>=b.r ; };
    
    // Computes intersection points between two balls with different centers
    std::vector<vector> intersections(const Ball<T>& b) const{
      std::vector<vector> intersections;
      intersections.reserve(2);
      
      // maybe throw an error if a ball with the same center ios provided
      if( c == b.c ){
        std::cerr << "Intersection of concentric balls detected" << std::endl;
        return intersections;
      }

      T d = c.getDistance(b.c);
      if( d>r+b.r || d < std::fabs(r-b.r))
        return intersections;

      T a = (d+(r*r-b.r*b.r)/d)/2, // b = d-a, // b results unused
        h=std::sqrt(r*r-a*a);
      // retrieving the vector v and its orthogonal (both have norm equal to d)
      vector v = b.c-c,
             v_norm = v.getOrthogonal();
      // retrieving the dummy point on the segment between the two balls' centers
      vector p1 = c + a/d*(b.c-c); // point distant a from c1 on the segment (c1)
      // computing the two intersections
      vector p2_1 = p1 - h/d*v_norm,
             p2_2 = p1 + h/d*v_norm ;

      intersections.push_back(p2_1);
      if( h > 0 )
        intersections.push_back(p2_2);
      
      return intersections;
    }
    
    // Computes the intersection arc (on the caller circumference) between two balls with different centers
    arc intersection_arc(const Ball<T>& b) const{
      std::vector<vector> points = intersections(b);
      
      if(points.size()<2) // if the balls are concentric, share just one point or are disconnected then there's no arc
        return arc(*this,1,0,0); // returning a null arc
      
      // if there are two intersection points then i can define an arc
      return arc(*this, points[0]-c, angle_vects<T>(points[0]-c,points[1]-c));
    }
};

#endif

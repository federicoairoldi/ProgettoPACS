#ifndef _ARC_
#define _ARC_

#include <ostream>
#include "Ball.h" 
#include "../MyGAL/Vector2.h"
#include "geomUtil.h"
using namespace mygal;

template<typename T> class CircArc;
template<typename T> class Ball;

// Insert an arc in a stream
template<typename T>
std::ostream& operator<<(std::ostream& os, const CircArc<T>& a){
  os << "center: " << a.b.center() << " radius: " << a.b.radius() << " v: " << a.v << " alpha: " 
     << a.alpha << " end: " << a.getEndVector();
  return os;
}

// Computes the set difference between two arcs on the same circumference
template<typename T>
std::vector<CircArc<T>> diff(const CircArc<T>& a1, const CircArc<T>& a2) { return a1.diff(a2); }

// Returns an arc rotated (counter-clockwise) by the angle theta
template<typename T>
CircArc<T> rotate_arc(const CircArc<T>& a, const T& theta){ return a.rotate(theta); }

template<typename T>
class CircArc{
  typedef Ball<T> ball;
  typedef Vector2<T> vector2;
  
  // FRIENDS
  friend std::ostream& operator<<<T>(std::ostream& os, const CircArc<T>& a);
  
  private:
    // ATTRIBUTES
    ball b; // ball
    vector2 v; // vector pointing to the start of the arc
    T alpha; // width of the angle defining the arc
  
  public:
    // CONSTRUCTORS
    CircArc() = delete; // deleting default constructor
    CircArc(const ball& b): b(b), v(vector2(1,0)), alpha(2*M_PI) {}; // defines an arc that covers the whole circle
    CircArc(const ball& b, const vector2& v, const T& alpha = 2*M_PI): b(b), v(normalize_vect(v)), alpha(alpha){ 
      if(alpha<0 || alpha>2*M_PI) 
        std::cerr << "Error! Magnitude for angle not in [0;2*pi]" << alpha; 
      if( v.getNorm() == 0 ) 
          std::cerr << "Error! vector v has null norm" << alpha; 
    };
    CircArc(const ball& b, const T& vx, const T& vy, const T& alpha = 2*M_PI): CircArc(b, vector2(vx,vy), alpha) {};
    CircArc(const T& cx, const T& cy,const T& r, const T& vx = 1, const T& vy = 0, const T& alpha = 2*M_PI): 
      b(cx, cy, r), v(vx, vy), alpha(alpha) {};
    
    // GETTERS
    ball getBall() const { return b; };
    T width() const { return alpha; };
    // Positions wrt the center of the ball
    vector2 getVector() const { return v; }; // a vector pointing the starting of the arc from center
    vector2 getEndVector() const { return rotate<T>(v,alpha); }; // a vector pointing the ending of the arc from center
    vector2 getMidVector() const { return rotate<T>(v,alpha/2); }; // a vector pointing the middle of the arc from center
    // Positions wrt the reference system (0,0)
    vector2 getPoint() const { return b.radius()*getVector()+b.center(); }; // a vector pointing the starting point of the arc
    vector2 getEndPoint() const { return b.radius()*getEndVector()+b.center(); }; // a vector pointing the ending point of the arc
    vector2 getMidPoint() const { return b.radius()*getMidVector()+b.center(); }; // a vector pointing the middle point of the arc
    // Returns whether or not the arc is degenerate (is just one point)
    bool isNULL() const { return alpha==0; }; // maybe do it with tollerance
    
    // OTHER METHODS
    // Computes the length of the arc
    T length() const { return alpha*b.radius(); };
    
    // Returns the angle of v, starting point of the arc (value in [0;2*PI))
    T theta1() const { 
      T theta1 = std::atan2(v.y, v.x);
      theta1+= theta1<0? 2*M_PI: 0;
      return theta1;
    };
    
    // Returns the angle of end point direction of the arc (value in (0;2*PI])
    T theta2() const { 
      T theta2 = theta1()+alpha;
      theta2-= theta2>2*M_PI? 2*M_PI: 0;
      return theta2;
    };
    
    // OTHER METHODS
    // Returns an arc rotated (counter-clockwise) by the angle theta
    CircArc<T> rotate_arc(const T& theta) const { return CircArc<T>(b, rotate<T>(v,theta), alpha); }
    
    // Computes the set difference between two arcs on the same circumference
    std::vector<CircArc> diff(const CircArc& a2) const{// arcs not on the same circumference
      if( b != a2.b ){
        std::cerr << "Error! arc difference with arcs in differet circles" << std::endl;
        return std::vector<CircArc>{*this};
      }
      
      // second arc is the whole circumference (the whole arc is removed)
      if(a2.alpha == 2*M_PI ) // I have to remove a whole circumference so the arc vanishes
        return std::vector<CircArc>();
        
      // second arc is a null arc (I have to do nothing)
      if(a2.alpha==0)
        return std::vector<CircArc>{*this};
      
      if(theta1()!=0){
        std::vector<CircArc> res = rotate_arc(-theta1()).diff(a2.rotate_arc(-theta1()));
        for(size_t i=0; i<res.size(); i++)
          res[i] = res[i].rotate_arc(theta1());
        return res;
      }
      
      // if we arrive here then we have that theta1 of first arc is 0.
      
      // case 1: 0 < alpha <= a2.theta1 < a2.theta2 (0 < alpha is always true by construction)
      if( alpha <= a2.theta1() && a2.theta1() < a2.theta2() )
        return std::vector<CircArc>{*this};
      
      // case 2: 0 < a2.theta1 < alpha <= a2.theta2
      if( 0 < a2.theta1() && a2.theta1() < alpha && alpha <= a2.theta2() )
        return std::vector<CircArc>{CircArc(b,v,a2.theta1())};
      
      // case 3: 0 < a2.theta1 < a2.theta2 < alpha
      if( 0 < a2.theta1() && a2.theta1() < a2.theta2() && a2.theta2() < alpha){
        // I have to split the arc in two parts
        if( alpha == 2*M_PI ) // sub-special case: instead of two arcs I just form one
          return std::vector<CircArc>{CircArc(b, a2.getEndVector(), alpha-a2.theta2()+a2.theta1())};
        // else
        return std::vector<CircArc>{CircArc(b, v, a2.theta1()), CircArc(b, a2.getEndVector(), alpha-a2.theta2())};
      }
      
      // case 4: 0 = a2.theta1 < alpha <= a2.theta2
      if( 0 == a2.theta1() && alpha <= a2.theta2() )
        return std::vector<CircArc>();
      
      // case 5: 0 = a2.theta1 < a2.theta2 < alpha 
      if( 0 == a2.theta1() && a2.theta2() < alpha )
        return std::vector<CircArc>{CircArc(b, a2.getEndVector(), alpha-a2.theta2())};
      
      // case 6: 0 < a2.theta2 < alpha <= a2.theta1
      if( 0 < a2.theta2() && a2.theta2() < alpha && alpha <= a2.theta1() )
        return std::vector<CircArc>{CircArc(b, a2.getEndVector(), alpha-a2.theta2())};
      
      // case 7: 0 < alpha <= a2.theta2 < a2.theta1
      if(alpha <= a2.theta2() && a2.theta2() < a2.theta1())
        return std::vector<CircArc>();;
      
      // case 8: 0 < a2.theta2 < a2.theta1 <= alpha
      if(a2.theta2() < a2.theta1() && a2.theta1() <= alpha)
        return std::vector<CircArc>{CircArc(b, a2.getEndVector(), a2.theta1()-a2.theta2())};
      
      // just in case something went wrong...
      std::cerr << "No case match!!" << std::endl;
      return std::vector<CircArc>();
    };
    
    // Removes from the caller arc the part of it that falls inside the given ball
    std::vector<CircArc> removeBall(const ball& b2) const {
      if( b2.containsBall(b) ) // I have to remove the whole arc, nothing remains
        return std::vector<CircArc>{};
      if( b.containsBall(b2) ) // I have to remove nothing, the arc remains intact
        return std::vector<CircArc>{*this};
      CircArc a2 = b.intersection_arc(b2);
      if( a2.alpha>0 )
        return diff(a2);
      return  std::vector<CircArc>{*this};
    }
};

#endif

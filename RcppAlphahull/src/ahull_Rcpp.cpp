#include <Rcpp.h>
#include <cmath>
#include "newClasses/Ball.h"
#include "newClasses/Line.h"
#include "newClasses/Segment.h"
#include "newClasses/HalfPlane.h"
#include "newClasses/CircArc.h"
#include "MyGAL/Vector2.h"
#include "utilities.h"
using namespace Rcpp;

Rcpp::NumericMatrix computeComplement(const Rcpp::NumericMatrix& mesh, const long double& alpha);

// starting from the complement matrix of the alpha hull returns a matrix containing arcs that describe
// the boundary
template<typename T>
std::list<CircArc<T>> getArcs(const Rcpp::NumericMatrix& complement){
  std::vector<Ball<T>> balls;
  std::vector<HalfPlane<T>> halfplanes;
  
  complement_matrix_to_vectors(complement, balls, halfplanes);
 
  std::list<CircArc<T>> arcs = union_boundary(balls); // construct the boundary of the union of balls

  // clamping arcs that are outside the convex hull (namely are in the halfplanes). By construction such
  // arcs have their starting and ending point at most on the boundary of one halfplane so I check if
  // the middle point is in any of them.
  for(typename std::vector<HalfPlane<T>>::const_iterator hp_it=halfplanes.cbegin(); hp_it!=halfplanes.cend(); hp_it++)
    arcs.remove_if([&](const CircArc<T>& a){ return hp_it->isIn(a.getMidPoint()); });
  
  return arcs;
}

// given the arcs that form the boundary of the alpha-hull return its length
template<typename T>
T ahull_boundary_length(const std::list<CircArc<T>>& arcs){
  T length = 0;
  for(typename std::list<CircArc<T>>::const_iterator arc=arcs.cbegin(); arc!=arcs.cend(); arc++)
    length+=arc->length();
  return length;
}

/* Function to retrieve a ahull object (almost) like the one returned by the alphahull function ahull,
 * but emplying previous results from the c++ library MyGAL
 */
// [[Rcpp::export(".computeAhullRcpp")]]
Rcpp::List computeAhullRcpp(Rcpp::List ashape) {
  // choosing the floating point representation to be used
  typedef long double real;

  // Retrieve the complement of the alpha-hull
  Rcpp::NumericMatrix mesh = as<Rcpp::List>(ashape["delvor.obj"])["mesh"];
  const real alpha = ashape["alpha"];

  // the complement is a union of open balls and open halfplanes
  Rcpp::NumericMatrix complement = computeComplement(mesh, alpha);

  // arcs of the boundary of the alpha-hull
  std::list<CircArc<real>> arcs = getArcs<real>(complement);
  
  // building arcs matrix
  Rcpp::NumericMatrix arcs_mat(arcs.size(), 8);
  std::vector<real> c1, c2, r, vx, vy, alpha_mid;
  
  for(typename std::list<CircArc<real>>::const_iterator arc=arcs.cbegin(); arc!=arcs.cend(); arc++){ 
    Vector2<real> v = arc->getMidVector(); // retrieving this vector in order to match alphahull description of arcs
    c1.push_back(arc->getBall().center().x);
    c2.push_back(arc->getBall().center().y);
    r.push_back(arc->getBall().radius());
    vx.push_back(v.x);
    vy.push_back(v.y);
    alpha_mid.push_back(arc->width()/2);
  }
  
  std::copy(c1.cbegin(), c1.cend(), arcs_mat(_,0).begin());
  std::copy(c2.cbegin(), c2.cend(), arcs_mat(_,1).begin());
  std::copy(r.cbegin(), r.cend(), arcs_mat(_,2).begin());
  std::copy(vx.cbegin(), vx.cend(), arcs_mat(_,3).begin());
  std::copy(vy.cbegin(), vy.cend(), arcs_mat(_,4).begin());
  std::copy(alpha_mid.cbegin(), alpha_mid.cend(), arcs_mat(_,5).begin());
  colnames(arcs_mat) = Rcpp::CharacterVector::create("c1", "c2", "r", "vx", "vy", "theta", "end1", "end2");
  
  // retriving xahull
  std::vector<real> x, y;
  for(typename std::list<CircArc<real>>::const_iterator arc=arcs.cbegin(); arc!=arcs.cend(); arc++){
    Vector2<real> start = arc->getPoint();
    x.push_back(start.x);
    y.push_back(start.y);
  }
  Rcpp::NumericMatrix xahull(arcs.size(), 2);
  std::copy(x.cbegin(), x.cend(), xahull(_,0).begin());
  std::copy(y.cbegin(), y.cend(), xahull(_,1).begin());
  
  // computing length of the alpha hull boundary
  real length = ahull_boundary_length<real>(arcs);
  
  Rcpp::List ahull = Rcpp::List::create(Rcpp::Named("arcs") = arcs_mat,
                                        Rcpp::Named("xahull") = xahull,
                                        Rcpp::Named("length") = length,
                                        Rcpp::Named("complement") = complement,
                                        Rcpp::Named("alpha") = ashape["alpha"],
                                        Rcpp::Named("ashape.obj") = ashape);
  ahull.attr("class") = "ahull";

  return ahull;
}

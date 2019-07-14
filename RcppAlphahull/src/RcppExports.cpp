// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// computeVoronoiRcpp
Rcpp::List computeVoronoiRcpp(const Rcpp::NumericVector x, const Rcpp::NumericVector y);
RcppExport SEXP _RcppAlphahull_computeVoronoiRcpp(SEXP xSEXP, SEXP ySEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::NumericVector >::type x(xSEXP);
    Rcpp::traits::input_parameter< const Rcpp::NumericVector >::type y(ySEXP);
    rcpp_result_gen = Rcpp::wrap(computeVoronoiRcpp(x, y));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_RcppAlphahull_computeVoronoiRcpp", (DL_FUNC) &_RcppAlphahull_computeVoronoiRcpp, 2},
    {NULL, NULL, 0}
};

RcppExport void R_init_RcppAlphahull(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}

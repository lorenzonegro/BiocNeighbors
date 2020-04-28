#include "distances.h"
#include <cmath>

double BNEuclidean::raw_distance(const double* x, const double* y, MatDim_t d) {
    double res=0;
    for (; d>0; --d, ++x, ++y) {
        const double tmp=*x - *y;
        res+=tmp*tmp;
    }
    return res;
}

double BNEuclidean::distance(const double* x, const double* y, MatDim_t d) {
    return normalize(raw_distance(x, y, d));
}

double BNEuclidean::normalize(double val) {
    return std::sqrt(val);
}

double BNEuclidean::unnormalize(double val) {
    return val*val;
}

double BNManhattan::raw_distance(const double* x, const double* y, MatDim_t d) {
    double res=0;
    for (; d>0; --d, ++x, ++y) {
        res+=std::fabs(*x - *y);
    }
    return res;
}

double BNManhattan::distance(const double*x, const double* y, MatDim_t d) {
    return raw_distance(x, y, d);
}

double BNManhattan::normalize(double val) {
    return val;
}

double BNManhattan::unnormalize(double val) {
    return val;
}

//d � il numero di elementi del vettore (quindi di algoritmi di clustering)
//Poi confronto ogni singolo elemento
double BNHamming::raw_distance(const double* x, const double* y, MatDim_t d) {
  double res=0;
  for (; d>0; --d, ++x, ++y) {
    if(*x!=*y){
      res++; 
    }
  }
  return res;
}

double BNHamming::distance(const double*x, const double* y, MatDim_t d) {
  return raw_distance(x, y, d);
}

double BNHamming::normalize(double val) {
  return val;
}

double BNHamming::unnormalize(double val) {
  return val;
}

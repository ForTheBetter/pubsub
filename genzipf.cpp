#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>

int zipf_variate(int N, double alpha)
{
 double sigma = 0.0;
 double sum = 0.0;
 int i;
 double r = (double)rand() / (double)RAND_MAX * 1.0;
 for(i = 0; i < N; i++) 
 {
    sigma += exp(-alpha*log(i+1));
  }
  for(i = 0; i < N; i++) 
  {
   sum += exp(-alpha*log(i+1));
   if (sum / sigma >= r) 
     return i + 1;
  }
  return N;
}

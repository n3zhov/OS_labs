//
// Created by nezhov on 13.12.2020.
//
#include <math.h>
#include "realization.h"

float Derivative(float A, float deltaX){
    float ans = (cosf(A+deltaX) - cosf(A))/deltaX;
    return ans;
}


#pragma clang diagnostic push
#pragma ide diagnostic ignored "DanglingPointers"
float E(int x){
    float res = powf((1+1/(float)x), (float)x);
    return res;
}
#pragma clang diagnostic pop



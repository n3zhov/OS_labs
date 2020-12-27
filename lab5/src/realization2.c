//
// Created by nezhov on 13.12.2020.
//
#include <math.h>
#include "realization.h"

float Derivative(float A, float deltaX){
    float ans = (cosf(A+deltaX) - cosf(A-deltaX))/(2*deltaX);
    return ans;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "DanglingPointers"
float E(int x){
    float res = 0;
    float factorial = 1;
    for(int i = 1; i < x; ++i){
        for(int j = 1; j <= i; ++j){
            factorial =  factorial*(float)j;
        }
        res += 1/factorial;
        factorial = 1;
    }
    return res;
}
#pragma clang diagnostic pop

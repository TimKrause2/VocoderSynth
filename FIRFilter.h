#pragma once

#include <memory>

struct FIRFilter
{
    std::unique_ptr<double[]> z;
    std::unique_ptr<double[]> a;
    int i_z;
    int order;
    FIRFilter(int order);
    double* CoeffPtr(void);
    float Evaluate(float x);
};

#pragma once

struct LowPass
{
    float z1;
    float z2;
    float a1;
    float a2;
    float b0;
    float b1;
    float b2;
    double sample_rate;
    LowPass(double sample_rate);
    void Set(float freq, float Q);
    float Evaluate(float x);
};

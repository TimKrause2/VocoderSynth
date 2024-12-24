#pragma once

#include "SincTable.h"

struct ImpulseGen
{
    float accumulator[N_SINC_WINDOW];
    int i_acc;
    float Tacc;
    float Tperiod;

    ImpulseGen(void);
    void Impulse(float alpha);
    float Evaluate(void);
};

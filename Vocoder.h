#pragma once
#include <memory>
#include "LPC.h"
#include "PoleFilter.h"

struct VocoderUnit
{
    std::unique_ptr<float[]> x;
    int N_window;
    int half_N_window;
    int i_x;
    float envelope;
    float denvelope;
    float gain;
    LPC lpc;
    PoleFilter filter;

    VocoderUnit(double sample_rate, bool even);
    float Evaluate(float x_ctl, float x_raw);
};

struct Vocoder
{
    std::unique_ptr<VocoderUnit> units[2];
    Vocoder(double sample_rate);
    float Evaluate(float x_ctl, float x_raw);
};

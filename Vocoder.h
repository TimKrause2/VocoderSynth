/*
VocoderSynth is a DSP plugin.

Copyright 2024 Tim Krause

This file is part of VocoderSynth

VocoderSynth is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.

VocoderSynth is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with VocoderSynth. If not, see
<https://www.gnu.org/licenses/>.

Contact: tim.krause@twkrause.ca

*/
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

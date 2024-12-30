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
#include <cmath>
#include "LowPass.h"

LowPass::LowPass(double sample_rate)
    : sample_rate(sample_rate)
{
    z1 = 0.0;
    z2 = 0.0;
}

void LowPass::Set(float freq, float Q)
{
    float l_omega = 2.0f*M_PIf*freq/sample_rate;
    float l_sin_omega = sinf(l_omega);
    float l_cos_omega = cosf(l_omega);
    float l_alpha = l_sin_omega / ( 2.0f * Q);
    float l_a0 = 1.0f + l_alpha;
    a1 = -2.0f * l_cos_omega / l_a0;
    a2 = (1.0f - l_alpha) / l_a0;
    b0 = (1.0f - l_cos_omega) / 2.0f / l_a0;
    b1 = (1.0f - l_cos_omega) / l_a0;
    b2 = (1.0f - l_cos_omega) / 2.0f / l_a0;
}

float LowPass::Evaluate(float x)
{
    float m = x - a1*z1 - a2*z2;
    float r = b0*m + b1*z1 + b2*z2;
    z2 = z1;
    z1 = m;
    return r;
}

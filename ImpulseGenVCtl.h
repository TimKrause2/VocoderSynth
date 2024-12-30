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
#include "ImpulseGen.h"
#include "SincTable.h"
#include "FIRFilter.h"
#include "LowPass.h"

class VocoderSynth;

struct Buffer
{
    std::unique_ptr<float[]> x;
    int N;
    int i_x;
    Buffer(int N);
    bool StoreComplete(float x);
};

struct ImpulseGenVCtl
{
    int N_window;
    int N_cor;
    int i_hi;
    int SS_rate;
    int N_ss_buffer;
    double sample_rate;
    bool noise;
    //std::unique_ptr<float[]> cor;
    std::unique_ptr<float[]> SS_buffer;
    std::unique_ptr<Buffer> buffers[2];
    ImpulseGen gen;
    LowPass lopass;
    LPC lpc;
    FIRFilter firFilter;
    VocoderSynth &plugin;

    ImpulseGenVCtl(double sample_rate, VocoderSynth &plugin);
    float Evaluate(float x);
private:
    void SuperSample(float *src, int Nsrc, float *dst);
    float PitchCorrelate(int lag);
    void Correlate(void);
};

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
#include "Vocoder.h"

#define T_WINDOW (1.0f/50.0f)

VocoderUnit::VocoderUnit(double sample_rate, bool even)
    : filter(N_ORDER)
{
    N_window = sample_rate*T_WINDOW;
    if(N_window&1)N_window++;
    half_N_window = N_window/2;
    if(even){
        i_x = 0;
    }else{
        i_x = half_N_window;
    }
    x.reset(new float[N_window]);
    for(int i=0;i<N_window;i++){
        x[i] = 0.0f;
    }
}

float VocoderUnit::Evaluate(float x_ctl, float x_raw)
{
    if(i_x == 0){
        envelope = 0.0f;
        denvelope = 1.0f/half_N_window;
    }else if(i_x == half_N_window){
        envelope = 1.0f;
        denvelope = -1.0f/half_N_window;
    }else{
        envelope += denvelope;
    }
    x[i_x] = x_ctl;
    i_x++;
    if(i_x == N_window){
        i_x = 0;
        // correlate
        lpc.Correlate(x.get(), N_window);
        if(lpc.R[0]>1e-9){
            lpc.LevinsonDurbin();
            double *csrc = lpc.Alpha();
            double *cdst = filter.CoeffPtr();
            for(int i=0;i<N_ORDER;i++){
                *(cdst++) = -*(csrc++);
            }
            gain = lpc.gain;
        }else{
            gain = 0.0f;
        }
    }
    float y = filter.Evaluate(x_raw)*envelope*gain;
    return y;
}

Vocoder::Vocoder(double sample_rate)
{
    units[0].reset(new VocoderUnit(sample_rate, true));
    units[1].reset(new VocoderUnit(sample_rate, false));
}

float Vocoder::Evaluate(float x_ctl, float x_raw)
{
    float y = units[0]->Evaluate(x_ctl, x_raw);
    y += units[1]->Evaluate(x_ctl, x_raw);
    return y;
}

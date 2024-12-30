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
#include "ImpulseGen.h"

ImpulseGen::ImpulseGen(void)
{
    for(int i=0;i<N_SINC_WINDOW;i++){
        accumulator[i] = 0.0f;
    }
    i_acc = 0;
    Tacc = 0.0f;
    Tperiod = 96.0f;
}

void ImpulseGen::Impulse(float alpha)
{
    int i_ss = (int)(alpha * N_SINC_SS);
    float *p_impulse = sincTable.data[i_ss];
    float *p_acc = &accumulator[i_acc];
    int N_loop1 = N_SINC_WINDOW - i_acc;
    int N_loop2 = i_acc;
    for(int i=0;i<N_loop1;i++){
        *(p_acc++) += *(p_impulse++);
    }
    if(N_loop2){
        p_acc = accumulator;
        for(int i=0;i<N_loop2;i++){
            *(p_acc++) += *(p_impulse++);
        }
    }
}

float ImpulseGen::Evaluate(void)
{
    float deltaT = Tacc - Tperiod;
    float result;
    if(deltaT <= -1.0f){
        Tacc += 1.0f;
    }else if(deltaT <= 0.0f){
        Impulse(-deltaT);
        Tacc = 1.0f + deltaT;
    }else{
        Impulse(0.0f);
        Tacc = 0.0f;
    }
    result = accumulator[i_acc];
    accumulator[i_acc] = 0.0f;
    i_acc++;
    if(i_acc == N_SINC_WINDOW)
        i_acc = 0;
    return result;
}

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
#include "FIRFilter.h"

FIRFilter::FIRFilter(int order)
    : order(order)
{
    z.reset(new double[order]);
    a.reset(new double[order]);
    for(int i=0;i<order;i++){
        z[i] = 0.0;
        a[i] = 0.0;
    }
    i_z = 0;
}

double *FIRFilter::CoeffPtr(void)
{
    return a.get();
}

float FIRFilter::Evaluate(float x)
{
    double l_fb = 0.0;
    long l_i_start = i_z - 1;
    if( l_i_start < 0 ) l_i_start+=order;

    long l_N_loop1 = l_i_start + 1;
    long l_N_loop2 = order - l_N_loop1;

    double *l_pa = a.get();
    double *l_pz =&z[l_i_start];

    long l_i;
    for( l_i=l_N_loop1-1; ; l_i--){
        l_fb += *l_pa * *l_pz;
        l_pa++;
        if( l_i==0 ) break;
        l_pz--;
    }

    if( l_N_loop2 ){
        l_pz = &z[order-1];
        for( l_i=l_N_loop2-1; ; l_i-- ){
            l_fb += *l_pa * *l_pz;
            if( l_i == 0 ) break;
            l_pa++;
            l_pz--;
        }
    }

    double l_y = (double)x + l_fb;
    if( !std::isfinite(l_y) ) l_y=0.0;
    z[ i_z ] = (double)x;
    if(++i_z==order)
        i_z = 0;

    return (float)l_y;

}

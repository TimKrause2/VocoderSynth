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
#include "LPC.h"

LPC::LPC(void)
{
    double *p = alpha_data;
    for(int i=0;i<N_ORDER;i++){
        alpha[i] = p;
        p += i + 1;
    }
}

void LPC::Correlate(float *src, int N)
{
    for(int i=0;i<(N_ORDER+1);i++){
        double c = 0.0;
        float *src0 = src;
        float *src1 = &src[i];
        float *src1_end = &src[N];
        for(;src1!=src1_end;src0++,src1++){
            c += (double)*src0 * (double)*src1;
        }
        c /= N;
        R[i] = c;
    }
}

void LPC::LevinsonDurbin(void)
{
    double l_E = R[0];
    long l_i;
    for( l_i=0; l_i<N_ORDER; l_i++ ){
        double l_k_num = R[l_i+1];
        long l_j;
        if( (l_i-1) >= 0 ){
            double *l_palpha = alpha[l_i-1];
            double *l_pR = &R[l_i];
            for( l_j=l_i-1; l_j>=0; l_j-- ){
                //for( l_j=0; l_j<=(l_i-1); l_j++ ){
                //l_k_num -= l_pData->m_palpha[l_i-1][l_j] * l_pData->m_R[l_i-l_j];
                l_k_num -= *l_palpha * *l_pR;
                l_palpha++;
                l_pR--;
            }
        }
        double l_k = l_k_num / l_E;
        alpha[l_i][l_i] = l_k;
        if( (l_i-1) >= 0 ){
            double *l_palphadst = alpha[l_i];
            double *l_palphasrc = alpha[l_i-1];
            double *l_palphasrck = &alpha[l_i-1][l_i-1];
            //for( l_j=0; l_j<=(l_i-1); l_j++ ){
            for( l_j = l_i-1; l_j>=0; l_j--){
                //l_pData->m_palpha[l_i][l_j] = l_pData->m_palpha[l_i-1][l_j] - l_k*l_pData->m_palpha[l_i-1][l_i-l_j-1];
                *l_palphadst = *l_palphasrc - l_k * *l_palphasrck;
                l_palphadst++;
                l_palphasrc++;
                l_palphasrck--;
            }
        }
        l_E *= (1 - l_k*l_k);
    }

    // program the gain of the filter
    gain = sqrt( l_E );
}

double* LPC::Alpha(void)
{
    return alpha[N_ORDER-1];
}


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
#include <cstdlib>
#include "ImpulseGenVCtl.h"
#include "VocoderSynth.h"

#define F_MIN 80.0f
#define F_MAX 500.0f
#define Q_LOWPASS 0.707f
#define N_COARSE 32

Buffer::Buffer(int N)
    :N(N),
    i_x(0)
{
    x.reset(new float[N]);
    for(int i=0;i<N;i++){
        x[i] = 0.0f;
    }
}

bool Buffer::StoreComplete(float x_in)
{
    x[i_x] = x_in;
    i_x++;
    bool result = false;
    if(i_x == N){
        i_x = 0;
        result = true;
    }
    return result;
}

ImpulseGenVCtl::ImpulseGenVCtl(double sample_rate, VocoderSynth &plugin)
    : sample_rate(sample_rate),
    noise(false),
    lopass(sample_rate),
    firFilter(N_ORDER),
    plugin(plugin)
{
    N_window = (int)ceilf(sample_rate/F_MIN);
    i_hi = (int)floorf(sample_rate/F_MAX);
    N_cor = N_window - i_hi + 1;
    if(sample_rate <= 50000.0){
        SS_rate = 4;
    }else if(sample_rate <= 100000.0){
        SS_rate = 2;
    }else{
        SS_rate = 1;
    }
    //cor.reset(new float[N_cor*SS_rate]);
    N_ss_buffer = N_window*2*SS_rate;
    SS_buffer.reset(new float[N_ss_buffer]);
    for(int i=0;i<2;i++){
        buffers[i].reset(new Buffer(N_window*2));
    }
    buffers[1]->i_x = N_window;
    lopass.Set(F_MAX, Q_LOWPASS);
}

float ImpulseGenVCtl::Evaluate(float x)
{
    for(int b=0;b<2;b++){
        if(buffers[b]->StoreComplete(x)){
            lpc.Correlate(buffers[b]->x.get(), buffers[b]->N);
            lpc.LevinsonDurbin();
            double *src = lpc.Alpha();
            double *dst = firFilter.CoeffPtr();
            for(int i=0;i<N_ORDER;i++){
                *(dst++) = -*(src++);
            }
            float *fsrc = buffers[b]->x.get();
            float *fsrc_end = &buffers[b]->x[buffers[b]->N];
            for(;fsrc!=fsrc_end;fsrc++){
                float y = firFilter.Evaluate(*fsrc);
                *fsrc = lopass.Evaluate(y);
            }
            SuperSample(buffers[b]->x.get(), buffers[b]->N, SS_buffer.get());
            Correlate();
        }
    }
    float y;
    if(noise){
        y = ((float)drand48()*2.0f-1.0f) * plugin.voice_noise_gain;
    }else{
        y = gen.Evaluate() * plugin.voice_impulse_gain;
    }
    return y;
}

void ImpulseGenVCtl::SuperSample(float *src, int Nsrc, float *dst)
{
    if(SS_rate == 1){
        float *src_end = &src[Nsrc];
        for(;src!=src_end;src++,dst++){
            *dst = *src;
        }
    }else{
        float l_cbuff[N_SINC_WINDOW];
        int l_i_cbuff = 0;
        int l_i_src = 0;
        for(;l_i_cbuff<N_SINC_WINDOW/2-1;l_i_cbuff++){
            l_cbuff[l_i_cbuff] = 0.0f;
        }
        for(;l_i_cbuff<N_SINC_WINDOW;l_i_cbuff++,l_i_src++){
            l_cbuff[l_i_cbuff] = src[l_i_src];
        }
        l_i_cbuff = 0;
        for(int s=0;s<Nsrc;s++){
            for(int ss=0;ss<SS_rate;ss++){
                int N_loop1 = N_SINC_WINDOW - l_i_cbuff;
                int N_loop2 = N_SINC_WINDOW - N_loop1;
                float *l_impulse = SS_sincTable.data[N_SINC_SS_SS*ss/SS_rate];
                float *l_pcbuff = &l_cbuff[l_i_cbuff];
                *dst = 0.0f;
                for(int i=0;i<N_loop1;i++){
                    *dst += *(l_impulse++) * *(l_pcbuff++);
                }
                if(N_loop2){
                    l_pcbuff = l_cbuff;
                    for(int i=0;i<N_loop2;i++){
                        *dst += *(l_impulse++) * *(l_pcbuff++);
                    }
                }
                dst++;
            }
            l_cbuff[l_i_cbuff] = (l_i_src<Nsrc)?src[l_i_src]:0.0f;
            l_i_cbuff++;
            if(l_i_cbuff==N_SINC_WINDOW)
                l_i_cbuff=0;
            l_i_src++;
        }
    }
}

float ImpulseGenVCtl::PitchCorrelate(int lag)
{
    float result = 0.0f;
    float *x0 = SS_buffer.get();
    float *x1 = &SS_buffer[lag];
    float *x1_end = &SS_buffer[N_ss_buffer];
    for(;x1!=x1_end;){
        result += *x0 * *x1;
        x0++;
        x1++;
    }
    return result;
}


void ImpulseGenVCtl::Correlate(void)
{
    int lag0=i_hi*SS_rate;
    int lag_last=N_window*SS_rate + SS_rate - 1;

    // coarse corelation
    float peek = 0.0f;
    int i_peek = lag0;
    for(int lag=lag0;lag<=lag_last;lag+=N_COARSE){
        float c = PitchCorrelate(lag);
        if(c > peek){
            peek = c;
            i_peek = lag;
        }
    }

    int n_coarse = N_COARSE/2;
    while(n_coarse){
        // test the lower correlate
        int fine_lag = i_peek - n_coarse;
        if(fine_lag >= lag0){
            float c = PitchCorrelate(fine_lag);
            if(c > peek){
                peek = c;
                i_peek = fine_lag;
            }
        }
        // test the upper correlate
        fine_lag = i_peek + n_coarse;
        if(fine_lag <= lag_last){
            float c = PitchCorrelate(fine_lag);
            if(c > peek){
                peek = c;
                i_peek = fine_lag;
            }
        }
        n_coarse/=2;
    }

    float cor0 = PitchCorrelate(0);

    if(peek/cor0 > 0.28f){
        noise = false;
        gen.Tperiod = (float)(i_peek)/SS_rate / plugin.voice_pitch_factor;
    }else{
        noise = true;
    }
}

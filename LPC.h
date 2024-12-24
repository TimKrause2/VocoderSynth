#pragma once

#define N_ORDER 48

struct LPC
{
    double *alpha[N_ORDER];
    double alpha_data[N_ORDER*(N_ORDER+1)/2];
    double R[N_ORDER+1];
    double gain;
    LPC(void);
    void Correlate(float *src, int N);
    void LevinsonDurbin(void);
    double *Alpha(void);
};

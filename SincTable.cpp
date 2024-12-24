#include "SincTable.h"
#include <cmath>

SincTable sincTable;
SS_SincTable SS_sincTable;

static float sinc(float x)
{
    if(fabs(x)<1e-9f){
        return 1.0f;
    }else{
        return sinf(x)/x;
    }
}

static float hamming(int n, int N)
{
    float a0 = 25.0f/46.0f;
    float a1 = 1.0f - a0;
    return a0 - a1*cosf(2.0f*M_PIf*n/(N-1));
}

SincTable::SincTable(void)
{
    for(int i_ss=0;i_ss<N_SINC_SS;i_ss++){
        float alpha = (float)i_ss/N_SINC_SS;
        for(int i_w=0;i_w<N_SINC_WINDOW;i_w++){
            float x = (float)(i_w-(N_SINC_WINDOW/2-1)) - alpha;
            data[i_ss][i_w] = sinc(M_PI*x)*hamming(i_w, N_SINC_WINDOW);
        }
    }
}

SS_SincTable::SS_SincTable(void)
{
    for(int i_ss=0;i_ss<N_SINC_SS_SS;i_ss++){
        float alpha = (float)i_ss/N_SINC_SS_SS;
        for(int i_w=0;i_w<N_SINC_WINDOW;i_w++){
            float x = (float)(i_w-(N_SINC_WINDOW/2-1)) - alpha;
            data[i_ss][i_w] = sinc(M_PI*x)*hamming(i_w, N_SINC_WINDOW);
        }
    }
}



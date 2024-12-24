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

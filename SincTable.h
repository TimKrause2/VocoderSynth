#pragma once

#define N_SINC_SS 1024
#define N_SINC_WINDOW 16

#define N_SINC_SS_SS 4

struct SincTable
{
    float data[N_SINC_SS][N_SINC_WINDOW];
    SincTable(void);
};

struct __attribute__ ((aligned(4096))) SS_SincTable
{
    float data[N_SINC_SS_SS][N_SINC_WINDOW];
    SS_SincTable(void);
};

extern SincTable sincTable;
extern SS_SincTable SS_sincTable;

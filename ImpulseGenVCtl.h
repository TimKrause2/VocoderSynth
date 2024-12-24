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

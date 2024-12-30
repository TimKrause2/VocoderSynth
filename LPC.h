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

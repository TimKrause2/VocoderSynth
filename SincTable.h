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

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
/*
 * Ports
 *  0 - input MIDI
 *  1 - input control audio
 *  2 - input raw audio
 *  3 - output synth audio
 *  4 - control 0 input raw audio select
 *  5 - control 1 input voice controlled impulse select
 *  6 - control 2 input voice controlled inpulse gain
 *  7 - control 3 input voice controlled noise gain
 *  8 - control 4 input voice controlled pitch offset
 *  9 - control 5 input impulse synth select
 * 10 - control 6 input impulse synth gain
 * 11 - control 7 input impulse synth pitch bend range
 *
 */
#pragma once
#include "ImpulseSynth.h"
#include "ImpulseGenVCtl.h"
#include "Vocoder.h"
#include "DCRemove.h"

enum ControlPorts
{
    CONTROL_RAW_ENABLE,
    CONTROL_VOICE_ENABLE,
    CONTROL_VOICE_IMPULSE_GAIN,
    CONTROL_VOICE_NOISE_GAIN,
    CONTROL_VOICE_PITCH_OFFSET,
    CONTROL_SYNTH_ENABLE,
    CONTROL_SYNTH_GAIN,
    CONTROL_SYNTH_BEND_RANGE,
    CONTROL_NCONTROLS
};

enum PortGroups
{
    PORT_MIDI_IN,
    PORT_CTL_AUDIO,
    PORT_RAW_AUDIO,
    PORT_AUDIO_OUT,
    PORT_CONTROL,
    PORT_NPORTS
};

class VocoderSynth
{
private:
    const LV2_Atom_Sequence* midi_in;
    float* ctl_audio;
    float* raw_audio;
    float* audio_out;
    const float* controls[CONTROL_NCONTROLS];
    DCRemove dcRemove;
    Vocoder vocoder;
    ImpulseSynth synth;
    ImpulseGenVCtl voice;
    void UpdateControls(void);
public:
    bool raw_audio_enabled;
    bool voice_enabled;
    float voice_impulse_gain;
    float voice_noise_gain;
    float voice_pitch_factor;
    bool synth_enabled;
    float synth_gain;
    float synth_pitch_bend;

    VocoderSynth(const double sample_rate, const LV2_Feature *const *features);
    void connectPort(const uint32_t port, void* data_location);
    void run(const uint32_t sample_count);
};

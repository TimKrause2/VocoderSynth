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
#include "VocoderSynth.h"

VocoderSynth::VocoderSynth(const double sample_rate, const LV2_Feature *const *features)
    :vocoder(sample_rate),
    synth(sample_rate, features, *this),
    voice(sample_rate, *this)
{

}

void VocoderSynth::UpdateControls(void)
{
    raw_audio_enabled = *controls[CONTROL_RAW_ENABLE]>0.0f;
    voice_enabled = *controls[CONTROL_VOICE_ENABLE]>0.0f;
    voice_impulse_gain = powf(10.0f, *controls[CONTROL_VOICE_IMPULSE_GAIN]/20.0f);
    voice_noise_gain = powf(10.0f, *controls[CONTROL_VOICE_NOISE_GAIN]/20.0f);
    voice_pitch_factor = powf(2.0f, *controls[CONTROL_VOICE_PITCH_OFFSET]/12.0f);
    synth_enabled = *controls[CONTROL_SYNTH_ENABLE]>0.0f;
    synth_gain = powf(10.0f, *controls[CONTROL_SYNTH_GAIN]/20.0f);
    synth_pitch_bend = *controls[CONTROL_SYNTH_BEND_RANGE];
}

void VocoderSynth::connectPort(const uint32_t port, void* data_location)
{
    switch(port){
    case PORT_MIDI_IN:
        midi_in = (const LV2_Atom_Sequence*) data_location;
        break;

    case PORT_CTL_AUDIO:
        ctl_audio = (float*)data_location;
        break;

    case PORT_RAW_AUDIO:
        raw_audio = (float*)data_location;
        break;

    case PORT_AUDIO_OUT:
        audio_out = (float*)data_location;
        break;

    default:
        if(port < (int)PORT_CONTROL + (int)CONTROL_NCONTROLS){
            controls[port - PORT_CONTROL] = (float*)data_location;
        }
    }
}

void VocoderSynth::run(const uint32_t sample_count)
{
    UpdateControls();
    if(synth_enabled){
        synth.EvaluateStart(midi_in);
    }
    float *ctl_p = ctl_audio;
    float *raw_p = raw_audio;
    float *out_p = audio_out;
    for(uint32_t s=0;s<sample_count;s++){
        float ctl_audio = dcRemove.Evaluate(*ctl_p);
        float vocoder_in = 0.0f;
        if(raw_audio_enabled){
            vocoder_in += *raw_p;
        }
        if(voice_enabled){
            vocoder_in += voice.Evaluate(ctl_audio);
        }
        if(synth_enabled){
            vocoder_in += synth.Evaluate();
        }
        *out_p = vocoder.Evaluate(ctl_audio, vocoder_in);
        ctl_p++;
        raw_p++;
        out_p++;
    }
}

/*
 * LV2 Interface functions
 */

static LV2_Handle instantiate (const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
    VocoderSynth *m;
    try {
        m = new VocoderSynth (sample_rate, features);
    }
    catch(...){
        m = nullptr;
    }
    return (LV2_Handle)m;
}

static void connect_port (LV2_Handle instance, uint32_t port, void *data_location)
{
    VocoderSynth* m = (VocoderSynth*) instance;
    if (m) m->connectPort (port, data_location);
}

static void activate (LV2_Handle instance)
{
    /* not needed here */
}

static void run (LV2_Handle instance, uint32_t sample_count)
{
    VocoderSynth* m = (VocoderSynth*) instance;
    if (m) m->run (sample_count);
}

static void deactivate (LV2_Handle instance)
{
    /* not needed here */
}

static void cleanup (LV2_Handle instance)
{
    VocoderSynth* m = (VocoderSynth*) instance;
    if (m) delete m;
}

static const void* extension_data (const char *uri)
{
    return NULL;
}

/* descriptor */
static LV2_Descriptor const descriptor =
    {
        "https://twkrause.ca/plugins/VocoderSynth",
        instantiate,
        connect_port,
        activate,
        run,
        deactivate,
        cleanup,
        extension_data
};

/* interface */
LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor (uint32_t index)
{
    if (index == 0) return &descriptor;
    else return NULL;
}




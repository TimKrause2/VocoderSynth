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
#include <iostream>
#include "ImpulseSynth.h"
#include "VocoderSynth.h"

Voice::Voice(double sample_rate, ImpulseSynth &synth):
    sample_rate(sample_rate),
    synth(synth)
{

}

void Voice::NoteOn(uint8_t note, uint8_t vel)
{
    Voice::note = note;
    Voice::vel = vel;
    freq0 = 440.0f*powf(2.0f,((float)note-69.0f)/12.0f);
}

void Voice::NoteOff(uint8_t vel)
{
    synth.DeactivateVoice(this);
}

float Voice::Evaluate(void)
{
    float freq = freq0 * powf(2.0f,synth.pitch_bend*synth.plugin.synth_pitch_bend/12.0f);
    gen.Tperiod = sample_rate / freq;
    return gen.Evaluate()*synth.plugin.synth_gain;
}

ImpulseSynth::ImpulseSynth(
    double sample_rate,
    const LV2_Feature *const *features,
    VocoderSynth &plugin)
    :
    sample_rate(sample_rate),
    plugin(plugin),
    pitch_bend(0.0f)
{
    LV2_URID_Map* map;
    const char* missing = lv2_features_query
        (
            features,
            LV2_URID__map, &map, true,
            NULL
            );

    if (missing){
        //std::cout << "map feature is missing" << std::endl;
        throw;
    }

    for(int i=0;i<N_VOICES;i++){
        alloc_voices[i].reset(new Voice(sample_rate, *this));
        avail_voices.push_back(alloc_voices[i].get());
    }
    for(int i=0;i<128;i++){
        keys[i] = {nullptr, false};
    }
    MidiEvent_URID = map->map(map->handle, LV2_MIDI__MidiEvent);
}

Voice* ImpulseSynth::ActivateVoice(void)
{
    Voice *voice;
    if(avail_voices.empty()){
        // take the oldest form the active list
        voice = active_voices.back();
        DeactivateVoice(voice);
    }
    // take from the available list
    voice = avail_voices.front();
    avail_voices.pop_front();
    active_voices.push_front(voice);
    voice->active_pos = active_voices.begin();
    return voice;
}

void ImpulseSynth::DeactivateVoice(Voice* voice)
{
    // remove from the active list
    active_voices.erase(voice->active_pos);
    // insert into the avail list
    avail_voices.push_front(voice);
    // remove from the key list
    keys[voice->note].voice = nullptr;
}

void ImpulseSynth::NoteOn(uint8_t note, uint8_t vel)
{
    Voice* voice;
    if(keys[note].voice){
        voice = keys[note].voice;
    }else{
        voice = ActivateVoice();
    }
    voice->NoteOn(note,vel);
    keys[note].voice = voice;
    keys[note].pressed = true;
}

void ImpulseSynth::NoteOff(uint8_t note, uint8_t vel)
{
    if(keys[note].pressed){
        if(keys[note].voice){
            keys[note].voice->NoteOff(vel);
        }
    }
    keys[note].pressed = false;
}

void ImpulseSynth::AllNotesOff(void)
{
    for(std::list<Voice*>::iterator it=active_voices.begin();it!=active_voices.end();)
    {
        Voice *voice = *it;
        it++;
        DeactivateVoice(voice);
    }
}

void ImpulseSynth::Bender(uint8_t lsb, uint8_t msb)
{
    short x = (lsb&0x7F)|((short)(msb&0x7F)<<7);
    pitch_bend = ((float)x-8192.0f)/8192.0f;
}


void ImpulseSynth::EvaluateStart(const LV2_Atom_Sequence *sequence)
{
    ImpulseSynth::sequence = sequence;
    event = lv2_atom_sequence_begin(&sequence->body);
    if(lv2_atom_sequence_is_end(&sequence->body,sequence->atom.size,event))
        event = nullptr;
    frame = 0;
}

void ImpulseSynth::NextEvent(void)
{
    if(!event)
        return;
    event = lv2_atom_sequence_next(event);
    if(lv2_atom_sequence_is_end(&sequence->body,sequence->atom.size,event))
        event = nullptr;
}

float ImpulseSynth::Evaluate(void)
{
    while(event && event->time.frames==frame)
    {
        if(event->body.type == MidiEvent_URID){
            const uint8_t* const msg = (const uint8_t*)(event + 1);
            const uint8_t type = lv2_midi_message_type(msg);
            switch(type){
            case LV2_MIDI_MSG_NOTE_ON:
                NoteOn(msg[1], msg[2]);
                break;

            case LV2_MIDI_MSG_NOTE_OFF:
                NoteOff(msg[1], msg[2]);
                break;

            case LV2_MIDI_MSG_CONTROLLER:
                if(msg[1]>=LV2_MIDI_CTL_ALL_SOUNDS_OFF)
                    AllNotesOff();
                break;

            case LV2_MIDI_MSG_BENDER:
                Bender(msg[1], msg[2]);
                break;

            default:
                break;
            }
        }
        NextEvent();
    }
    frame++;
    float y=0.0f;
    for(auto &voice:active_voices){
        y += voice->Evaluate();
    }
    return y;
}

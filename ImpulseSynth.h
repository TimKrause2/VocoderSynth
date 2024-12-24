#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <lv2/atom/atom.h>
#include <lv2/urid/urid.h>
#include <lv2/midi/midi.h>
#include <lv2/core/lv2_util.h>
#include <lv2/atom/util.h>
#include "ImpulseGen.h"

#define N_VOICES 4

class ImpulseSynth;
class VocoderSynth;

struct Voice
{
    uint8_t note;
    uint8_t vel;
    double sample_rate;
    float freq0;
    ImpulseSynth &synth;
    std::list<Voice*>::iterator active_pos;
    ImpulseGen gen;

    Voice(double sample_rate, ImpulseSynth &synth);
    void NoteOn(uint8_t note, uint8_t vel);
    void NoteOff(uint8_t vel);
    float Evaluate(void);
};

struct Key
{
    Voice* voice;
    bool pressed;
};

class ImpulseSynth
{
private:
    std::list<Voice*> avail_voices;
    std::list<Voice*> active_voices;
    std::unique_ptr<Voice> alloc_voices[N_VOICES];
    Key keys[128];
    const LV2_Atom_Sequence *sequence;
    LV2_Atom_Event* event;
    void NextEvent(void);
    uint32_t frame;
    LV2_URID MidiEvent_URID;
    double sample_rate;
public:
    VocoderSynth &plugin;
    float pitch_bend;
    ImpulseSynth(double sample_rate, const LV2_Feature *const *features, VocoderSynth &plugin);
    Voice* ActivateVoice(void);
    void DeactivateVoice(Voice *voice);
    void NoteOn(uint8_t note, uint8_t vel);
    void NoteOff(uint8_t note, uint8_t vel);
    void AllNotesOff(void);
    void Bender(uint8_t lsb, uint8_t msb);
    void EvaluateStart(const LV2_Atom_Sequence *sequence);
    float Evaluate(void);
};

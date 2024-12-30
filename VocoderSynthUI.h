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

#include <string>
#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include "xputty.h"
#include "xwidget.h"
#include "VocoderSynth.h"

class VocoderSynthUI
{
private:
    LV2UI_Write_Function write_function;
    LV2UI_Controller controller;
    Widget_t* dial;
    Widget_t* controls[CONTROL_NCONTROLS];
    Widget_t* box;
    Xputty main;

public:
    VocoderSynthUI (LV2UI_Write_Function write_function, LV2UI_Controller controller, void* parentXWindow, std::string bundlePath);
    ~VocoderSynthUI();
    LV2UI_Widget getTopLevelWidget();
    Xputty* getXputty();
    void portEvent(uint32_t port_index, uint32_t buffer_size, uint32_t format, const void *buffer);
    static void valueChangedCallback (void* obj, void* data);
    static void exposeCallback (void* obj, void* data);
    void set_colors(Xputty * app);
};

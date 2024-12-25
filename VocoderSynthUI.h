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
};

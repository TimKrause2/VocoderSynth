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
#include <iostream>
#include "cairo/cairo.h"
#include "VocoderSynthUI.h"
#include <stdexcept>
#include "xadjustment.h"
#include "xputty.h"
#include "xwidget.h"
#include "xwidgets.h"

enum CtlTypes
{
    CTYPE_KNOB,
    CTYPE_BUTTON,
};

struct CtlProps
{
    CtlTypes ctlType;
    int data;
    const char *label;
    int x;
    int y;
    int width;
    int height;
    float std_value;
    float value;
    float min_value;
    float max_value;
    float step;
    CL_type cl_type;
};

CtlProps ctlProps[CONTROL_NCONTROLS] =
{
    {
        CTYPE_BUTTON, PORT_CONTROL + CONTROL_RAW_ENABLE,
        "Raw Enable",
        0, 0, 119, 29,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, CL_CONTINUOS
    },
    {
        CTYPE_BUTTON, PORT_CONTROL + CONTROL_VOICE_ENABLE,
        "Voice Enable",
        120, 0, 119, 29,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, CL_CONTINUOS
    },
    {
        CTYPE_KNOB, PORT_CONTROL + CONTROL_VOICE_IMPULSE_GAIN,
        "Impulse",
        120, 50, 59, 79,
        25.0f, 25.0f, 0.0f, 30.0f, 0.1f, CL_CONTINUOS
    },
    {
        CTYPE_KNOB, PORT_CONTROL + CONTROL_VOICE_NOISE_GAIN,
        "Noise",
        180, 50, 59, 79,
        3.0f, 3.0f, 0.0f, 30.0f, 0.1f, CL_CONTINUOS
    },
    {
        CTYPE_KNOB, PORT_CONTROL + CONTROL_VOICE_PITCH_OFFSET,
        "Pitch Shift",
        150, 130, 59, 79,
        0.0f, 0.0f, -12.0f, 12.0f, 0.1f, CL_CONTINUOS
    },
    {
        CTYPE_BUTTON, PORT_CONTROL + CONTROL_SYNTH_ENABLE,
        "Synth Enable",
        240, 0, 119, 29,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, CL_CONTINUOS
    },
    {
        CTYPE_KNOB, PORT_CONTROL + CONTROL_SYNTH_GAIN,
        "Gain",
        240, 50, 59, 79,
        20.0f, 20.0f, 0.0f, 30.0f, 0.1f, CL_CONTINUOS
    },
    {
        CTYPE_KNOB, PORT_CONTROL + CONTROL_SYNTH_BEND_RANGE,
        "Bend",
        300, 50, 59, 79,
        0.0f, 0.0f, 0.0f, 12.0f, 0.1f, CL_CONTINUOS
    }
};

struct LblProps
{
    const char *label;
    int x,y,width,height;
};

#define N_LABELS 3

LblProps lblProps[N_LABELS] =
{
    {
        "Raw Enable", 0, 30, 119, 19,
    },
    {
        "Voice Enable", 120, 30, 119, 19,
    },
    {
        "Synth Enable", 240, 30, 119, 19,
    }
};

VocoderSynthUI::VocoderSynthUI (LV2UI_Write_Function write_function, LV2UI_Controller controller, void* parentXWindow, std::string bundlePath):
    write_function(write_function),
    controller(controller)
{
    main_init (&main);
    box = create_window (&main, reinterpret_cast<Window>(parentXWindow), 0, 0, 360, 210);
    box->parent_struct = this;
    box->label = "VocoderSynth";
    box->func.expose_callback = exposeCallback;

    for(int i=0;i<CONTROL_NCONTROLS;i++){
        CtlProps *p = &ctlProps[i];
        if(p->ctlType==CTYPE_KNOB){
            controls[i] = add_knob(box,
                p->label, p->x, p->y, p->width, p->height);
            set_adjustment(controls[i]->adj,
                p->std_value, p->value, p->min_value, p->max_value,
                p->step, p->cl_type);
        }else if(p->ctlType==CTYPE_BUTTON){
            controls[i] = add_on_off_button(box,
                p->label, p->x, p->y, p->width, p->height);
            adj_set_value(controls[i]->adj, p->value);
        }
        controls[i]->parent_struct = this;
        controls[i]->data = p->data;
        controls[i]->func.value_changed_callback = valueChangedCallback;
    }

    for(int i=0;i<N_LABELS;i++){
        LblProps *p = &lblProps[i];
        Widget_t *l = add_label(box, p->label, p->x, p->y,
            p->width, p->height);
    }


    widget_show_all (box);
}

VocoderSynthUI::~VocoderSynthUI()
{
    main_quit (&main);
}

LV2UI_Widget VocoderSynthUI::getTopLevelWidget()
{
    return reinterpret_cast<LV2UI_Widget>(box->widget);
}

Xputty* VocoderSynthUI::getXputty()
{
    return &main;
}

void VocoderSynthUI::portEvent(uint32_t port_index, uint32_t buffer_size, uint32_t format, const void *buffer)
{
    if (format == 0)
    {
        const float value = *static_cast<const float*>(buffer);
        if(port_index>=PORT_CONTROL &&
            port_index<PORT_CONTROL+CONTROL_NCONTROLS)
        {
            adj_set_value(controls[port_index - PORT_CONTROL]->adj, value);
        }
    }
}

void VocoderSynthUI::valueChangedCallback (void* obj, void* data)
{
    Widget_t* widget = static_cast<Widget_t*>(obj);
    VocoderSynthUI* ui = static_cast<VocoderSynthUI*>(widget->parent_struct);
    if (ui)
    {
        float value = adj_get_value (widget->adj);
        ui->write_function (ui->controller, widget->data, sizeof(value), 0, &value);
    }
}

void VocoderSynthUI::exposeCallback (void* obj, void* data)
{
    Widget_t* widget = static_cast<Widget_t*>(obj);
    cairo_set_source_rgb (widget->crb, 0.25, 0.25, 0.25 );
    cairo_paint (widget->crb);
}

static LV2UI_Handle instantiate(const struct LV2UI_Descriptor *descriptor, const char *plugin_uri, const char *bundle_path, LV2UI_Write_Function write_function, LV2UI_Controller controller, LV2UI_Widget *widget, const LV2_Feature *const *features)
{
    if (strcmp (plugin_uri, "https://twkrause.ca/plugins/VocoderSynth") != 0) return nullptr;
    
    void* parentXWindow = nullptr;
    for (int i = 0; features[i]; ++i)
    {
        if (strcmp (features[i]->URI, LV2_UI__parent) == 0) parentXWindow = features[i]->data;
    }

    if (!parentXWindow)
    {
        std::cerr << "Required feature LV2_UI__parent not provided" << std::endl;
        return nullptr;
    }

    VocoderSynthUI* ui;
    try {ui = new VocoderSynthUI(write_function, controller, parentXWindow, std::string (bundle_path));}
    catch (std::exception& exc)
    {
        std::cerr << "UI instantiation failed." << std::endl;
        return nullptr;
    }

    *widget = ui->getTopLevelWidget();
    return (LV2UI_Handle) ui;
}

static void cleanup (LV2UI_Handle ui)
{
    VocoderSynthUI* myUi = static_cast<VocoderSynthUI*>(ui);
    if (myUi) delete myUi;
}

static void port_event(LV2UI_Handle ui, uint32_t port_index, uint32_t buffer_size, uint32_t format, const void *buffer)
{
    VocoderSynthUI* myUi = static_cast<VocoderSynthUI*>(ui);
    if (myUi) myUi->portEvent(port_index, buffer_size, format, buffer);
}

static int ui_idle (LV2UI_Handle ui)
{
    VocoderSynthUI* myUi = static_cast<VocoderSynthUI*>(ui);
    run_embedded (myUi->getXputty());
    return 0;
}

static const void * extension_data (const char *uri)
{
    static const LV2UI_Idle_Interface idle = { ui_idle };
    if (strcmp (uri, LV2_UI__idleInterface) == 0) return &idle;
    return nullptr;
}

static const LV2UI_Descriptor ui_descriptor =
{
    "https://twkrause.ca/plugins/VocoderSynth#ui",
    instantiate,
    cleanup,
    port_event,
    extension_data
};

LV2_SYMBOL_EXPORT const LV2UI_Descriptor * 	lv2ui_descriptor (uint32_t index)
{
    switch (index)
    {
        case 0:     return &ui_descriptor;
        default:    return 0;
    }
}


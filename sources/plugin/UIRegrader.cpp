/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jean Pierre Cimalando
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "UIRegrader.hpp"
#include "SharedRegrader.hpp"
#include "Window.hpp"
#include "paramids.h"
#include "ui/BitmapCache.h"
#include "ui/Slider.h"
#include "ui/CheckBox.h"

// jpc: original LFO frequency control formula was: (((10.**x)-1.)*1.05556)+0.05

namespace Igorski {

// -----------------------------------------------------------------------
// Init / Deinit

UIRegrader::UIRegrader()
    : UI(933, 501)
{
    fSliderById = new Slider *[kNumParameters]{};
    fCheckBoxById = new CheckBox *[kNumParameters]{};

    fParameterRanges = new ParameterRangesSimple[kNumParameters];

    for (unsigned i = 0; i < kNumParameters; ++i) {
        Parameter param;
        SharedRegrader::InitParameter(i, param);
        fParameterRanges[i] = ParameterRangesSimple{param.ranges.def, param.ranges.min, param.ranges.max};
    }

    // Delay module
    createSlider(kDelayTimeId, 155, 111, 134, 21);
    createSlider(kDelayFeedbackId, 155, 135, 134, 21);
    createSlider(kDelayMixId, 155, 159, 134, 21);
    createCheckBox(kDelayHostSyncId, 154, 182, 21, 21);

    // BitCrusher module
    createSlider(kBitResolutionId, 463, 111, 134, 21/*, kControlInverted*/);
    createSlider(kLFOBitResolutionId, 463, 135, 134, 21/*, kControlLogarithmic*/);
    createSlider(kLFOBitResolutionDepthId, 463, 159, 134, 21);
    createCheckBox(kBitResolutionChainId, 462, 182, 21, 21);

    // Decimator module
    createSlider(kDecimatorId, 771, 111, 134, 21);
    createSlider(kLFODecimatorId, 771, 135, 134, 21);
    createCheckBox(kDecimatorChainId, 770, 158, 21, 21);

    // Filter module
    createSlider(kFilterCutoffId, 155, 311, 134, 21);
    createSlider(kFilterResonanceId, 155, 335, 134, 21);
    createSlider(kLFOFilterId, 155, 359, 134, 21/*, kControlLogarithmic*/);
    createSlider(kLFOFilterDepthId, 155, 383, 134, 21);
    createCheckBox(kFilterChainId, 154, 406, 21, 21);

    // Flanger module
    createSlider(kFlangerRateId, 463, 311, 134, 21, kControlLogarithmic);
    createSlider(kFlangerWidthId, 463, 335, 134, 21);
    createSlider(kFlangerFeedbackId, 463, 359, 134, 21);
    createSlider(kFlangerDelayId, 463, 383, 134, 21);
    createCheckBox(kFlangerChainId, 462, 406, 21, 21);

    for (unsigned i = 0; i < kNumParameters; ++i) {
        ParameterRangesSimple range = fParameterRanges[i];
        setParameterValue(i, range.def);
    }
}

UIRegrader::~UIRegrader() {
    while (!fSubwidgets.empty()) {
        delete fSubwidgets.back();
        fSubwidgets.pop_back();
    }

    delete[] fSliderById;
    delete[] fCheckBoxById;

    delete[] fParameterRanges;
}

// -----------------------------------------------------------------------
// DSP/Plugin callbacks

/**
  A parameter has changed on the plugin side.
  This is called by the host to inform the UI about parameter changes.
*/
void UIRegrader::parameterChanged(uint32_t index, float value) {
    DISTRHO_SAFE_ASSERT_RETURN(index < kNumParameters, );

    if (Slider *ctl = fSliderById[index]) {
        ctl->setValue(value, CControl::kDoNotNotify);
    }
    if (CheckBox *ctl = fCheckBoxById[index]) {
        ctl->setValue(value, CControl::kDoNotNotify);
    }
}

/**
  Optional callback to inform the UI about a sample rate change on the plugin side.
*/
void UIRegrader::sampleRateChanged(double newSampleRate) {
    (void)newSampleRate;
}

// -----------------------------------------------------------------------
// Widget callbacks


/**
  A function called to draw the view contents.
*/
void UIRegrader::onDisplay() {

    cairo_t* cr = getParentWindow().getGraphicsContext().cairo;

    cairo_set_line_width(cr, 1.0);

    cairo_surface_t *bg = BitmapCache::load(150);
    cairo_set_source_surface(cr, bg, 0, 0);
    cairo_paint(cr);
}


// -----------------------------------------------------------------------

void UIRegrader::createSlider(int id, int x, int y, int w, int h, int flags)
{
    cairo_surface_t *imgBody = BitmapCache::load(151);
    cairo_surface_t *imgHandle = BitmapCache::load(152);

    Slider *ctl = new Slider(imgBody, imgHandle, this);
    fSubwidgets.push_back(ctl);

    ctl->setTag(id);
    fSliderById[id] = ctl;

    ParameterRangesSimple range = fParameterRanges[id];
    if ((flags & kControlLogarithmic) != 0)
        ctl->setLogarithmic(true);
    if ((flags & kControlInverted) == 0)
        ctl->setValueBounds(range.min, range.max);
    else
        ctl->setValueBounds(range.max, range.min);

    ctl->addListener(this);
    ctl->setAbsolutePos(x, y);
    ctl->setSize(w, h);
}

void UIRegrader::createCheckBox(int id, int x, int y, int w, int h, int flags)
{
    CheckBox *ctl = new CheckBox(0x000000ff, 0x09f447ff, this);
    fSubwidgets.push_back(ctl);

    ctl->setTag(id);
    fCheckBoxById[id] = ctl;

    (void)flags;

    ParameterRangesSimple range = fParameterRanges[id];
    ctl->setValueBounds(range.min, range.max);

    ctl->addListener(this);
    ctl->setAbsolutePos(x, y);
    ctl->setSize(w, h);
}

// -----------------------------------------------------------------------

void UIRegrader::controlValueChanged(CControl &ctl)
{
    int id = ctl.getTag();
    if (id >= 0 && id < kNumParameters)
        setParameterValue(id, ctl.getValue());
}

void UIRegrader::controlBeganChangeGesture(CControl &ctl)
{
    int id = ctl.getTag();
    if (id >= 0 && id < kNumParameters)
        this->editParameter(id, true);
}

void UIRegrader::controlEndedChangeGesture(CControl &ctl)
{
    int id = ctl.getTag();
    if (id >= 0 && id < kNumParameters)
        this->editParameter(id, false);
}

// -----------------------------------------------------------------------

}

// -----------------------------------------------------------------------

UI* DISTRHO::createUI() {
    return new Igorski::UIRegrader;
}

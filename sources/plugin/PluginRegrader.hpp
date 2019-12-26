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

#ifndef PLUGIN_REGRADER_H
#define PLUGIN_REGRADER_H

#include "DistrhoPlugin.hpp"
#include "regraderprocess.h"
#include "global.h"

namespace Igorski {

// -----------------------------------------------------------------------

class PluginRegrader : public Plugin {
public:
    PluginRegrader();
    ~PluginRegrader();

protected:
    // -------------------------------------------------------------------
    // Information

    const char* getLabel() const noexcept override {
        return "Regrader";
    }

    const char* getDescription() const override {
        return "A delay effect where the repeats degrade in resolution";
    }

    const char* getMaker() const noexcept override {
        return "Igorski";
    }

    const char* getHomePage() const override {
        return "https://www.igorski.nl/download/regrader--degenerative-delay-processor";
    }

    const char* getLicense() const noexcept override {
        return "https://spdx.org/licenses/MIT";
    }

    uint32_t getVersion() const noexcept override {
        return d_version(0, 1, 0);
    }

    // Go to:
    //
    // http://service.steinberg.de/databases/plugin.nsf/plugIn
    //
    // Get a proper plugin UID and fill it in here!
    //
    // jpc: below value is not a "proper UID", but who cares
    int64_t getUniqueId() const noexcept override {
        return d_cconst('R', 'g', 'd', 'r');
    }

    // -------------------------------------------------------------------
    // Init

    void initParameter(uint32_t index, Parameter& parameter) override;

    // -------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override;
    void setParameterValue(uint32_t index, float value) override;

    // -------------------------------------------------------------------
    // Optional

    // Optional callback to inform the plugin about a sample rate change.
    void sampleRateChanged(double newSampleRate) override;

    // -------------------------------------------------------------------
    // Process

    void activate() override;

    void run(const float**, float** outputs, uint32_t frames) override;


    // -------------------------------------------------------------------

private:
    float fDelayTime;
    float fDelayHostSync;
    float fDelayFeedback;
    float fDelayMix;

    float fBitResolution;
    float fBitResolutionChain;
    float fLFOBitResolution;
    float fLFOBitResolutionDepth;

    float fDecimator;
    float fDecimatorChain;
    float fLFODecimator;

    float fFilterChain;
    float fFilterCutoff;
    float fFilterResonance;
    float fLFOFilter;
    float fLFOFilterDepth;

    float fFlangerChain;
    float fFlangerRate;
    float fFlangerWidth;
    float fFlangerFeedback;
    float fFlangerDelay;

    float outputGain; // for visualizing output gain in DAW

    Igorski::RegraderProcess* regraderProcess;

    // synchronize the processors model with UI led changes

    void syncModel();

    // -------------------------------------------------------------------

    struct ParameterRangesSimple
    {
        float def, min, max;
    };

    ParameterRangesSimple *fParameterRanges;

    // -------------------------------------------------------------------

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginRegrader)
};

// -----------------------------------------------------------------------

}

#endif  // #ifndef PLUGIN_REGRADER_H

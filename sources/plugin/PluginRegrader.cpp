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

#include "PluginRegrader.hpp"
#include "SharedRegrader.hpp"
#include "paramids.h"
#include "calc.h"
#include <math.h>

namespace Igorski {

// -----------------------------------------------------------------------

PluginRegrader::PluginRegrader()
    : Plugin(kNumParameters, 0, 0)
    , fDelayTime( 0.125f )
    , fDelayHostSync( 1.f )
    , fDelayFeedback( 0.2f )
    , fDelayMix( .5f )
    , fBitResolution( 1.f )
    , fBitResolutionChain( 1.f )
    , fLFOBitResolution( .0f )
    , fLFOBitResolutionDepth( .75f )
    , fDecimator( 1.f )
    , fDecimatorChain( 0.f )
    , fLFODecimator( 0.f )
    , fFilterChain( 1.f )
    , fFilterCutoff( .5f )
    , fFilterResonance( 1.f )
    , fLFOFilter( 0.f )
    , fLFOFilterDepth( 0.5f )
    , fFlangerChain( 0.f )
    , fFlangerRate( 0.f )
    , fFlangerWidth( 0.f )
    , fFlangerFeedback( 0.f )
    , fFlangerDelay( 0.f )
    , outputGain( 0.f )
{
    fParameterRanges = new ParameterRangesSimple[kNumParameters];

    for (unsigned i = 0; i < kNumParameters; ++i) {
        Parameter param;
        SharedRegrader::InitParameter(i, param);
        fParameterRanges[i] = ParameterRangesSimple{param.ranges.def, param.ranges.min, param.ranges.max};
    }

    sampleRateChanged(getSampleRate());
}

PluginRegrader::~PluginRegrader()
{
    // free all allocated resources
    delete regraderProcess;
    delete[] fParameterRanges;
}

// -----------------------------------------------------------------------
// Init

void PluginRegrader::initParameter(uint32_t index, Parameter& parameter) {
    SharedRegrader::InitParameter(index, parameter);
}

// -----------------------------------------------------------------------
// Internal data

/**
  Optional callback to inform the plugin about a sample rate change.
*/
void PluginRegrader::sampleRateChanged(double newSampleRate) {
    regraderProcess = new RegraderProcess( 2, newSampleRate );

    for (unsigned i = 0; i < kNumParameters; ++i) {
        ParameterRangesSimple range = fParameterRanges[i];
        setParameterValue(i, range.def);
    }
}

/**
  Get the current value of a parameter.
*/
float PluginRegrader::getParameterValue(uint32_t index) const {
    float value;

    switch (index) {
    case kDelayTimeId:             // delay time
        value = fDelayTime;
        break;
    case kDelayHostSyncId:         // delay host sync
        value = fDelayHostSync;
        break;
    case kDelayFeedbackId:         // delay feedback
        value = fDelayFeedback;
        break;
    case kDelayMixId:              // delay mix
        value = fDelayMix;

        break;
    case kBitResolutionId:         // bit resolution
        value = fBitResolution;
        break;
    case kBitResolutionChainId:    // bit resolution pre/post delay mix
        value = fBitResolutionChain;
        break;
    case kLFOBitResolutionId:      // bit resolution LFO rate
        value = fLFOBitResolution;
        break;
    case kLFOBitResolutionDepthId: // depth for bit resolution LFO
        value = fLFOBitResolutionDepth;

        break;
    case kDecimatorId:             // decimator
        value = fDecimator;
        break;
    case kDecimatorChainId:        // decimator pre/post delay mix
        value = fDecimatorChain;
        break;
    case kLFODecimatorId:          // decimator LFO rate
        value = fLFODecimator;

        break;
    case kFilterChainId:           // filter pre/post delay mix
        value = fFilterChain;
        break;
    case kFilterCutoffId:          // filter cutoff
        value = fFilterCutoff;
        break;
    case kFilterResonanceId:       // filter resonance
        value = fFilterResonance;
        break;
    case kLFOFilterId:             // filter LFO rate
        value = fLFOFilter;
        break;
    case kLFOFilterDepthId:        // depth for filter LFO
        value = fLFOFilterDepth;

        break;
    case kFlangerChainId:          // flanger pre/post delay mix
        value = fFlangerChain;
        break;
    case kFlangerRateId:           // flanger LFO rate
        value = fFlangerRate;
        break;
    case kFlangerWidthId:          // flanger width
        value = fFlangerWidth;
        break;
    case kFlangerFeedbackId:       // flanger feedback
        value = fFlangerFeedback;
        break;
    case kFlangerDelayId:          // flanger delay
        value = fFlangerDelay;
        break;

    case kVuPPMId:                 // for the Vu value return to host
        value = outputGain;
        break;

    default:
        DISTRHO_SAFE_ASSERT_RETURN(false, 0.0f);
    }

    ParameterRangesSimple range = fParameterRanges[index];
    return value * (range.max - range.min) + range.min;
}

/**
  Change a parameter value.
*/
void PluginRegrader::setParameterValue(uint32_t index, float value) {
    DISTRHO_SAFE_ASSERT_RETURN(index < kNumParameters, );

    ParameterRangesSimple range = fParameterRanges[index];
    value = (value - range.min) / (range.max - range.min);

    switch (index) {
    case kDelayTimeId:             // delay time
        fDelayTime = value;
        break;
    case kDelayHostSyncId:         // delay host sync
        fDelayHostSync = value;
        break;
    case kDelayFeedbackId:         // delay feedback
        fDelayFeedback = value;
        break;
    case kDelayMixId:              // delay mix
        fDelayMix = value;
        break;

    case kBitResolutionId:         // bit resolution
        fBitResolution = value;
        break;
    case kBitResolutionChainId:    // bit resolution pre/post delay mix
        fBitResolutionChain = value;
        break;
    case kLFOBitResolutionId:      // bit resolution LFO rate
        fLFOBitResolution = value;
        break;
    case kLFOBitResolutionDepthId: // depth for bit resolution LFO
        fLFOBitResolutionDepth = value;
        break;

    case kDecimatorId:             // decimator
        fDecimator = value;
        break;
    case kDecimatorChainId:        // decimator pre/post delay mix
        fDecimatorChain = value;
        break;
    case kLFODecimatorId:          // decimator LFO rate
        fLFODecimator = value;
        break;

    case kFilterChainId:           // filter pre/post delay mix
        fFilterChain = value;
        break;
    case kFilterCutoffId:          // filter cutoff
        fFilterCutoff = value;
        break;
    case kFilterResonanceId:       // filter resonance
        fFilterResonance = value;
        break;
    case kLFOFilterId:             // filter LFO rate
        fLFOFilter = value;
        break;
    case kLFOFilterDepthId:        // depth for filter LFO
        fLFOFilterDepth = value;
        break;

    case kFlangerChainId:          // flanger pre/post delay mix
        fFlangerChain = value;
        break;
    case kFlangerRateId:           // flanger LFO rate
        fFlangerRate = value;
        break;
    case kFlangerWidthId:          // flanger width
        fFlangerWidth = value;
        break;
    case kFlangerFeedbackId:       // flanger feedback
        fFlangerFeedback = value;
        break;
    case kFlangerDelayId:          // flanger delay
        fFlangerDelay = value;
        break;

    case kVuPPMId:                 // for the Vu value return to host
        outputGain = value;
        break;

    default:
        DISTRHO_SAFE_ASSERT_RETURN(false, );
    }

    syncModel();
}

// -----------------------------------------------------------------------
// Process

void PluginRegrader::activate() {
    // plugin is activated
}



void PluginRegrader::run(const float** inputs, float** outputs, uint32_t frames) {
    TimePosition timePos = getTimePosition();
    TimePosition::BarBeatTick bbt = timePos.bbt;

    if ( bbt.valid ) {
        regraderProcess->setTempo(
            bbt.beatsPerMinute, bbt.beatsPerBar, bbt.beatType
        );
    }


    //-------------------------------------
    //---Process Audio---------------------
    //-------------------------------------

    int32 numInChannels  = DISTRHO_PLUGIN_NUM_INPUTS;
    int32 numOutChannels = DISTRHO_PLUGIN_NUM_OUTPUTS;

    // process the incoming sound!
    regraderProcess->process<float>(
        const_cast<float **>(inputs), outputs, numInChannels, numOutChannels,
        frames, frames * sizeof(float)
    );

    // output flags
    outputGain = regraderProcess->limiter->getLinearGR();
}

// -----------------------------------------------------------------------

void PluginRegrader::syncModel()
{
    regraderProcess->syncDelayToHost = Calc::toBool( fDelayHostSync );
    regraderProcess->setDelayTime( fDelayTime );
    regraderProcess->setDelayFeedback( fDelayFeedback );
    regraderProcess->setDelayMix( fDelayMix );

    regraderProcess->bitCrusherPostMix = Calc::toBool( fBitResolutionChain );
    regraderProcess->decimatorPostMix  = Calc::toBool( fDecimatorChain );
    regraderProcess->filterPostMix     = Calc::toBool( fFilterChain );
    regraderProcess->flangerPostMix    = Calc::toBool( fFlangerChain );

    regraderProcess->bitCrusher->setAmount( fBitResolution );
    regraderProcess->bitCrusher->setLFO( fLFOBitResolution, fLFOBitResolutionDepth );
    regraderProcess->decimator->setBits( ( int )( fDecimator * 32.f ));
    regraderProcess->decimator->setRate( fLFODecimator );
    regraderProcess->filter->updateProperties( fFilterCutoff, fFilterResonance, fLFOFilter, fLFOFilterDepth );

    regraderProcess->flanger->setRate( fFlangerRate );
    regraderProcess->flanger->setWidth( fFlangerWidth );
    regraderProcess->flanger->setFeedback( fFlangerFeedback );
    regraderProcess->flanger->setDelay( fFlangerDelay );
}

// -----------------------------------------------------------------------

}

// -----------------------------------------------------------------------

Plugin* DISTRHO::createPlugin() {
    return new Igorski::PluginRegrader;
}

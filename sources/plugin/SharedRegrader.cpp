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

#include "SharedRegrader.hpp"
#include "global.h"
#include "paramids.h"

namespace Igorski {
namespace SharedRegrader {

void InitParameter(uint32_t index, Parameter& parameter)
{
    parameter.ranges = ParameterRanges(0.0, 0.0, 1.0);
    parameter.hints = kParameterIsAutomable;

    switch (index) {
    case kDelayTimeId:             // delay time
        parameter.symbol = "DelayTime";
        parameter.name = "Delay time";
        parameter.ranges.def = 0.125;
        parameter.unit = "s";
        break;
    case kDelayHostSyncId:         // delay host sync
        parameter.symbol = "DelayHostSync";
        parameter.name = "Delay host sync";
        parameter.ranges.def = 1.0;
        parameter.hints |= kParameterIsBoolean|kParameterIsInteger;
        break;
    case kDelayFeedbackId:         // delay feedback
        parameter.symbol = "DelayFeedback";
        parameter.name = "Delay feedback";
        parameter.ranges.def = 0.2;
        break;
    case kDelayMixId:              // delay mix
        parameter.symbol = "DelayMix";
        parameter.name = "Delay mix";
        parameter.ranges.def = 0.5;
        break;

    case kBitResolutionId:         // bit resolution
        parameter.symbol = "BitResolution";
        parameter.name = "Bit resolution";
        parameter.ranges = ParameterRanges(16.0, 1.0, 16.0);
        break;
    case kBitResolutionChainId:    // bit resolution pre/post delay mix
        parameter.symbol = "BitResolutionChain";
        parameter.name = "BitCrusher chain";
        parameter.ranges.def = 1.0;
        parameter.hints |= kParameterIsBoolean|kParameterIsInteger;
        break;
    case kLFOBitResolutionId:      // bit resolution LFO rate
        parameter.symbol = "LFOBitResolution";
        parameter.name = "Bit LFO rate";
        parameter.ranges = ParameterRanges(Igorski::VST::MIN_LFO_RATE(), Igorski::VST::MIN_LFO_RATE(), Igorski::VST::MAX_LFO_RATE());
        parameter.unit = "Hz";
        //parameter.hints |= kParameterIsLogarithmic;
        break;
    case kLFOBitResolutionDepthId: // depth for bit resolution LFO
        parameter.symbol = "LFOBitResolutionDepth";
        parameter.name = "Bit LFO depth";
        parameter.ranges.def = 0.75;
        break;

    case kDecimatorId:             // decimator
        parameter.symbol = "Decimator";
        parameter.name = "Decimator resolution";
        parameter.ranges = ParameterRanges(32.0, 0.0, 32.0);
        parameter.hints |= kParameterIsInteger;
        break;
    case kDecimatorChainId:        // decimator pre/post delay mix
        parameter.symbol = "DecimatorChain";
        parameter.name = "Decimator chain";
        parameter.hints |= kParameterIsBoolean|kParameterIsInteger;
        break;
    case kLFODecimatorId:          // decimator LFO rate
        parameter.symbol = "LFODecimator";
        parameter.name = "Decimator rate";
        break;

    case kFilterChainId:           // filter pre/post delay mix
        parameter.symbol = "FilterChain";
        parameter.name = "Filter chain";
        parameter.hints |= kParameterIsBoolean|kParameterIsInteger;
        break;
    case kFilterCutoffId:          // filter cutoff
        parameter.symbol = "FilterCutoff";
        parameter.name = "Filter cutoff";
        parameter.ranges = ParameterRanges(0.5 * Igorski::VST::FILTER_MAX_FREQ, Igorski::VST::FILTER_MIN_FREQ, Igorski::VST::FILTER_MAX_FREQ);
        parameter.unit = "Hz";
        break;
    case kFilterResonanceId:       // filter resonance
        parameter.symbol = "FilterResonance";
        parameter.name = "Filter resonance";
        parameter.ranges = ParameterRanges(Igorski::VST::FILTER_MAX_RESONANCE, Igorski::VST::FILTER_MIN_RESONANCE, Igorski::VST::FILTER_MAX_RESONANCE);
        parameter.unit = "dB";
        break;
    case kLFOFilterId:             // filter LFO rate
        parameter.symbol = "LFOFilter";
        parameter.name = "Filter LFO rate";
        parameter.ranges = ParameterRanges(Igorski::VST::MIN_LFO_RATE(), Igorski::VST::MIN_LFO_RATE(), Igorski::VST::MAX_LFO_RATE());
        parameter.unit = "Hz";
        //parameter.hints |= kParameterIsLogarithmic;
        break;
    case kLFOFilterDepthId:        // depth for filter LFO
        parameter.symbol = "LFOFilterDepth";
        parameter.name = "Filter LFO depth";
        parameter.ranges.def = 0.5;
        break;

    case kFlangerChainId:          // flanger pre/post delay mix
        parameter.symbol = "FlangerChain";
        parameter.name = "Flanger chain";
        parameter.hints |= kParameterIsBoolean|kParameterIsInteger;
        break;
    case kFlangerRateId: {         // flanger LFO rate
        parameter.symbol = "FlangerRate";
        parameter.name = "Flanger LFO rate";
        parameter.ranges = ParameterRanges(Igorski::VST::MIN_LFO_RATE(), Igorski::VST::MIN_LFO_RATE(), Igorski::VST::MAX_LFO_RATE());
        parameter.unit = "Hz";
        parameter.hints |= kParameterIsLogarithmic;
        break;
    }
    case kFlangerWidthId:          // flanger width
        parameter.symbol = "FlangerWidth";
        parameter.name = "Flanger width";
        break;
    case kFlangerFeedbackId:       // flanger feedback
        parameter.symbol = "FlangerFeedback";
        parameter.name = "Flanger feedback";
        break;
    case kFlangerDelayId:          // flanger delay
        parameter.symbol = "FlangerDelay";
        parameter.name = "Flanger delay";
        parameter.ranges = ParameterRanges(0.1, 0.1, 1.0);
        break;

    case kVuPPMId:                 // for the Vu value return to host
        parameter.symbol = "VuPPM";
        parameter.name = "Output gain";
        parameter.hints |= kParameterIsOutput;
        break;

    default:
        DISTRHO_SAFE_ASSERT_RETURN(false, );
    }
}

}
}

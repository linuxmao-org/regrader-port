#include "global.h"

#pragma message("TODO(jpc): the sample rate")
// note: made it non-static, to make sure we are addressing the same variable
//       from diffent translation units
// note: it's desirable to make this non global, so that it can differ between
//       different instances
float Igorski::VST::SAMPLE_RATE = 44100;

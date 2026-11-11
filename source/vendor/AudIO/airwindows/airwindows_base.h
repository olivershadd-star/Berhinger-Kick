#pragma once

#include <stdint.h>

typedef int VstInt32;
typedef int VstPlugCategory;
typedef int audioMasterCallback;

#ifndef M_PI
#define M_PI AudIO::Math<float>::pi
#endif

static constexpr int kVstMaxProgNameLen   = 64;
static constexpr int kVstMaxParamStrLen   = 64;
static constexpr int kVstMaxProductStrLen = 64;
static constexpr int kVstMaxVendorStrLen  = 64;

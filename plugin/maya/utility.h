// Copyright (C) 2021 Hamed Sabri
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef PLUGIN_MAYA_UTILITY_H
#define PLUGIN_MAYA_UTILITY_H

#include <maya/MFnTransform.h>

#include <lib3mf_implicit.hpp>

#include <string_view>

#include <maya/MApiNamespace.h>

namespace M3mf {

// print lib3mf version
void printLibVersion();

// message box
void messageBox(std::string_view caption, std::string_view message);

// convert Maya unit system to 3MF
Lib3MF::eModelUnit convertMayaToModelUnit();

// scale multiplier
double getMayaScaleMultiplier();

// converts Lib3MFTransform to MMatrix
MMatrix convert(const sLib3MFTransform& xform);

// converts MMatrix to Lib3MFTransform
sLib3MFTransform convert(const MMatrix& xform);

// connect source to destination plug
MStatus connect(const MPlug& src, const MPlug& dst, const bool clear);

} // namespace M3mf

#endif // PLUGIN_MAYA_UTILITY_H
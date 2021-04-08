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

#include "utility.h"
#include "debugOstream.h"

#include <maya/MColor.h>
#include <maya/MDGModifier.h>
#include <maya/MDistance.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MString.h>

namespace M3mf {

void printLibVersion()
{
    try {
        auto wrapper = Lib3MF::CWrapper::loadLibrary();

        Lib3MF_uint32 nMajor, nMinor, nMicro;
        wrapper->GetLibraryVersion(nMajor, nMinor, nMicro);

        std::string verInfo { "Maya3MF using Lib3MF Version " + std::to_string(nMajor) + "." + std::to_string(nMinor) + "." + std::to_string(nMicro) };

        std::string sPreReleaseInfo;
        if (wrapper->GetPrereleaseInformation(sPreReleaseInfo)) {
            verInfo += "-" + sPreReleaseInfo;
        }

        std::string sBuildInfo;
        if (wrapper->GetBuildInformation(sBuildInfo)) {
            verInfo += "+" + sBuildInfo;
        }

        MGlobal::displayInfo(verInfo.c_str());
    } catch (std::exception& e) {
        MGlobal::displayInfo(e.what());
    }
}

void messageBox(std::string_view caption, std::string_view message)
{
    MString messageBoxCommand = ("confirmDialog -title \"" + MString(caption.data()) + "\" -message \"" + MString(message.data()) + "\" -button \"Ok\" -defaultButton \"Ok\"");

    MGlobal::executeCommand(messageBoxCommand);
}

Lib3MF::eModelUnit convertMayaToModelUnit()
{
    Lib3MF::eModelUnit unit { Lib3MF::eModelUnit::CentiMeter };

    switch (MDistance::uiUnit()) {
    case MDistance::Unit::kFeet:
        unit = Lib3MF::eModelUnit::Foot;
        break;

    case MDistance::Unit::kMeters:
        unit = Lib3MF::eModelUnit::Meter;
        break;

    case MDistance::Unit::kCentimeters:
        unit = Lib3MF::eModelUnit::CentiMeter;
        break;

    case MDistance::Unit::kInches:
        unit = Lib3MF::eModelUnit::Inch;
        break;

    case MDistance::Unit::kMillimeters:
        unit = Lib3MF::eModelUnit::MilliMeter;
        break;
    }

    return unit;
}

MMatrix convert(const sLib3MFTransform& xform)
{
    // row major order
    MMatrix m;

    m.matrix[0][0] = xform.m_Fields[0][0];
    m.matrix[0][1] = xform.m_Fields[0][1];
    m.matrix[0][2] = xform.m_Fields[0][2];

    m.matrix[1][0] = xform.m_Fields[1][0];
    m.matrix[1][1] = xform.m_Fields[1][1];
    m.matrix[1][2] = xform.m_Fields[1][2];

    m.matrix[2][0] = xform.m_Fields[2][0];
    m.matrix[2][1] = xform.m_Fields[2][1];
    m.matrix[2][2] = xform.m_Fields[2][2];

    m.matrix[3][0] = xform.m_Fields[3][0];
    m.matrix[3][1] = xform.m_Fields[3][1];
    m.matrix[3][2] = xform.m_Fields[3][2];

    return m;
}

sLib3MFTransform convert(const MMatrix& xform)
{
    sLib3MFTransform m;

    m.m_Fields[0][0] = xform.matrix[0][0];
    m.m_Fields[0][1] = xform.matrix[0][1];
    m.m_Fields[0][2] = xform.matrix[0][2];

    m.m_Fields[1][0] = xform.matrix[1][0];
    m.m_Fields[1][1] = xform.matrix[1][1];
    m.m_Fields[1][2] = xform.matrix[1][2];

    m.m_Fields[2][0] = xform.matrix[2][0];
    m.m_Fields[2][1] = xform.matrix[2][1];
    m.m_Fields[2][2] = xform.matrix[2][2];

    m.m_Fields[3][0] = xform.matrix[3][0];
    m.m_Fields[3][1] = xform.matrix[3][1];
    m.m_Fields[3][2] = xform.matrix[3][2];

    return m;
}

double getMayaScaleMultiplier()
{
    // CM ---> other units
    double scaleMult { 1.0 };

    switch (MDistance::uiUnit()) {
    case MDistance::Unit::kFeet:
        scaleMult = 0.0328084;
        break;
    case MDistance::Unit::kMeters:
        scaleMult = 0.01;
        break;
    case MDistance::Unit::kCentimeters:
        scaleMult = 1.0;
        break;
    case MDistance::Unit::kInches:
        scaleMult = 0.393701;
        break;
    case MDistance::Unit::kMillimeters:
        scaleMult = 10.0;
        break;
    }

    return scaleMult;
}

// connect source to destination plug
MStatus connect(const MPlug& src, const MPlug& dst, const bool clear)
{
    MStatus status { MS::kSuccess };
    MDGModifier mod;

    if (clear) {
        MPlugArray plgArray;
        dst.connectedTo(plgArray, true, false, &status);

        for (uint32_t i = 0; i < plgArray.length(); ++i) {
            status = mod.disconnect(plgArray[i], dst);
        }
    }

    status = mod.connect(src, dst);
    if (!status) {
        status.perror("connection failed!");
        return status;
    }

    mod.doIt();

    return status;
}

} // namespace M3mf

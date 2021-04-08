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

#include "exportTranslator.h"
#include "export.h"

#include <maya/MStatus.h>

namespace M3mf {

void* ExportTranslator::creator()
{
    return new ExportTranslator;
}

MStatus ExportTranslator::writer(const MFileObject& file, const MString& options, FileAccessMode mode)
{
    bool exportSelected { false };

    if ((mode == MPxFileTranslator::kExportAccessMode) || (mode == MPxFileTranslator::kSaveAccessMode)) {
        exportSelected = false;
    } else if (mode == MPxFileTranslator::kExportActiveAccessMode) {
        exportSelected = true;
    }

    Export exporter;
    auto sucess = exporter.write(file.fullName().asChar(), exportSelected);

    return sucess ? MS::kSuccess : MS::kFailure;
}

bool ExportTranslator::haveReadMethod() const
{
    return false;
}

bool ExportTranslator::haveWriteMethod() const
{
    return true;
}

MString ExportTranslator::defaultExtension() const
{
    return "3mf";
}

MString ExportTranslator::filter() const
{
    return "*.3mf", "*.3mf; *.3MF";
}

} // namespace M3mf
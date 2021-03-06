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

#include "importTranslator.h"
#include "import.h"

#include <maya/MObjectArray.h>
#include <maya/MStringArray.h>

namespace M3mf {

void* ImportTranslator::creator()
{
    return new ImportTranslator;
}

MStatus ImportTranslator::reader(const MFileObject& file, const MString& optionsStr, FileAccessMode mode)
{
    Import importer;
    auto success = importer.read(file.resolvedFullName().asChar());
    return success ? MS::kSuccess : MS::kFailure;

    return MS::kSuccess;
}

bool ImportTranslator::haveReadMethod() const
{
    return true;
}

bool ImportTranslator::haveWriteMethod() const
{
    return false;
}

MString ImportTranslator::defaultExtension() const
{
    return "3mf";
}

MString ImportTranslator::filter() const
{
    return "*.3mf", "*.3mf; *.3MF";
}

} // namespace M3mf
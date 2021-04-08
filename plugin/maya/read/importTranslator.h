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

#ifndef PLUGIN_MAYA_IMPORT_TRANSLATOR_H
#define PLUGIN_MAYA_IMPORT_TRANSLATOR_H

#include <maya/MFileObject.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MString.h>

namespace M3mf {

class ImportTranslator final : public MPxFileTranslator
{
public:
    ImportTranslator() = default;
    ~ImportTranslator() = default;

    ImportTranslator(ImportTranslator&& other) = delete;
    ImportTranslator(const ImportTranslator&) = delete;
    ImportTranslator& operator=(const ImportTranslator&) = delete;
    ImportTranslator& operator=(ImportTranslator&&) = delete;

    static void* creator();

    MStatus reader(const MFileObject& file, const MString& optionsStr, FileAccessMode) override;

    bool haveReadMethod() const override;
    bool haveWriteMethod() const override;
    MString defaultExtension() const override;
    MString filter() const override;
};

} // namespace M3mf

#endif // PLUGIN_MAYA_IMPORT_TRANSLATOR_H
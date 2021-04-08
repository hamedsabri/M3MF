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

#ifndef PLUGIN_MAYA_EXPORT_COMMAND_H
#define PLUGIN_MAYA_EXPORT_COMMAND_H

#include <maya/MPxCommand.h>

namespace M3mf {

class ExportCommand final : public MPxCommand
{
public:
    ExportCommand() = default;
    ~ExportCommand() = default;

    ExportCommand(ExportCommand&& other) = delete;
    ExportCommand(const ExportCommand&) = delete;
    ExportCommand& operator=(const ExportCommand&) = delete;
    ExportCommand& operator=(ExportCommand&&) = delete;

    static void* creator();

    static void cleanup();
    static MSyntax createSyntax();

    MStatus doIt(const MArgList&) override;
    MStatus undoIt() override;
    bool isUndoable() const override
    {
        return true;
    };
};

} // namespace M3mf

#endif // PLUGIN_MAYA_EXPORT_COMMAND_H

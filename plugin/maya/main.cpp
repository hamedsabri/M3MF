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

#include "read/importTranslator.h"
#include "read/importCommand.h"

#include "write/exportCommand.h"
#include "write/exportTranslator.h"

#include "utility.h"

#include <maya/MFnPlugin.h>
#include <maya/MObject.h>
#include <maya/MString.h>

namespace {
const char* kAUTHOR = "Hamed Sabri";
const char* kVERSION = "1.0";
const char* kREQUIRED_API_VERSION = "Any";
const MString kIMPORT_COMMAND_NAME = "maya3MFImport";
const MString kIMPORT_TRANSLATOR_NAME = "3MF Import";
const MString kEXPORT_COMMAND_NAME = "maya3MFExport";
const MString kEXPORT_TRANSLATOR_NAME = "3MF Export";
} // namespace

MStatus initializePlugin(MObject obj)
{
    MStatus status { MS::kSuccess };

    MFnPlugin plugin(obj, kAUTHOR, kVERSION, kREQUIRED_API_VERSION);

    // print lib3mf version
    M3mf::printLibVersion();

    // register import command/translator
    status = plugin.registerCommand(kIMPORT_COMMAND_NAME, M3mf::ImportCommand::creator, M3mf::ImportCommand::createSyntax);

    if (!status) {
        status.perror("registerCommand(" + kIMPORT_COMMAND_NAME + ")");
        return status;
    }

    status = plugin.registerFileTranslator(kIMPORT_TRANSLATOR_NAME, "", M3mf::ImportTranslator::creator, "", "", false);

    if (!status) {
        status.perror("Failed to register ImportTranslator!");
        return status;
    }

    // register export command/translator
    status = plugin.registerCommand(kEXPORT_COMMAND_NAME, M3mf::ExportCommand::creator, M3mf::ExportCommand::createSyntax);

    if (!status) {
        status.perror("registerCommand(" + kIMPORT_COMMAND_NAME + ")");
        return status;
    }

    status = plugin.registerFileTranslator(kEXPORT_TRANSLATOR_NAME, "none", M3mf::ExportTranslator::creator);

    if (!status) {
        status.perror("Failed to register ExportTranslator!");
        return status;
    }

    return status;
}

MStatus uninitializePlugin(MObject obj)
{
    MStatus status { MS::kSuccess };
    MFnPlugin plugin(obj);

    // import
    status = plugin.deregisterCommand(kIMPORT_COMMAND_NAME);
    if (!status) {
        status.perror("deregisterCommand(" + kIMPORT_COMMAND_NAME + ")");
        return status;
    }

    status = plugin.deregisterFileTranslator(kIMPORT_TRANSLATOR_NAME);

    if (!status) {
        status.perror("Failed to deregister ImportTranslator!");
        return status;
    }

    // export
    status = plugin.deregisterCommand(kEXPORT_COMMAND_NAME);
    if (!status) {
        status.perror("deregisterCommand(" + kEXPORT_COMMAND_NAME + ")");
        return status;
    }

    status = plugin.deregisterFileTranslator(kEXPORT_TRANSLATOR_NAME);

    if (!status) {
        status.perror("Failed to deregister ExportTranslator!");
        return status;
    }

    return status;
}

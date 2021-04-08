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

#include "exportCommand.h"
#include "export.h"

#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MFileObject.h>
#include <maya/MGlobal.h>
#include <maya/MSyntax.h>

namespace M3mf {

void* ExportCommand::creator()
{
    return new ExportCommand();
}

void ExportCommand::cleanup()
{
}

MStatus ExportCommand::doIt(const MArgList& args)
{
    MStatus status;
    MArgDatabase argData(syntax(), args, &status);

    if (!argData.isFlagSet("-f", &status)) {
        MGlobal::displayError("ExportCommand: \"file\" argument must be set!");
        return MS::kFailure;
    }

    MString fileName;
    argData.getFlagArgument("-f", 0, fileName);

    MFileObject file;
    file.setRawName(fileName);

    Export exporter;
    auto success = exporter.write(file.fullName().asChar(), true);
    return success ? MS::kSuccess : MS::kFailure;
}

MSyntax ExportCommand::createSyntax()
{
    MSyntax syntax;
    syntax.addFlag("-f", "-file", MSyntax::kString);
    return syntax;
}

MStatus ExportCommand::undoIt()
{
    return MS::kSuccess;
}

} // namespace M3mf

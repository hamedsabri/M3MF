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

#ifndef PLUGIN_MAYA_IMPORT_H
#define PLUGIN_MAYA_IMPORT_H

#include "types.h"

#include <lib3mf_implicit.hpp>

#include <maya/MStatus.h>

#include <string_view>

namespace M3mf {

class Import final
{
public:
    Import() = default;
    ~Import() = default;

    Import(Import&& other) = delete;
    Import(const Import&) = delete;
    Import& operator=(const Import&) = delete;
    Import& operator=(Import&&) = delete;

    bool read(std::string_view fileName);

private:
    MStatus createMeshObject(const Lib3MF::PWrapper& wrapper, 
                             const Lib3MF::PModel& model, 
                             const Lib3MF::PMeshObject& mesh, 
                             const sLib3MFTransform& transform, 
                             std::string_view objectName);

    MStatus assignVertexColors(const MObject& object, 
                               const SetOfIndexAndVertColor& vertColorSet);

    MStatus assignLamabertShader(const MObject& object, 
                                 std::string_view shaderName, 
                                 const MColor& color);
};

class ResourceIDCheck
{
public:
    ResourceIDCheck(Lib3MF::PModel model);
    bool isRessourceIDValid(uint32_t ID);

private:
    std::set<uint32_t> _resourceIDs;
};

} // namespace M3mf

#endif // PLUGIN_MAYA_IMPORT_H
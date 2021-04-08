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

#ifndef PLUGIN_3DSMAX_THREEMFIMPORT_H
#define PLUGIN_3DSMAX_THREEMFIMPORT_H

#include "resource.h"
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <maxtypes.h>

#include <impexp.h>
#include <direct.h>
#include <commdlg.h>

#include <lib3mf_implicit.hpp>

#include <mesh.h>

#include <set>
#include <string_view>

#define ThreeMFImport_CLASS_ID Class_ID(0xa3ce3a79, 0x6e0cb4f3)

static const TCHAR _className[] = _T("ThreeMFImport");

extern HINSTANCE hInstance;

namespace M3mf {

class ThreeMFImport : public SceneImport
{
public:
    ThreeMFImport();
    virtual ~ThreeMFImport();

    int ExtCount() override;
    const TCHAR* Ext(int n) override;
    const TCHAR* LongDesc() override;
    const TCHAR* ShortDesc() override;
    const TCHAR* AuthorName() override;
    const TCHAR* CopyrightMessage() override;
    const TCHAR* OtherMessage1() override;
    const TCHAR* OtherMessage2() override;
    unsigned int Version() override;
    void ShowAbout(HWND hWnd) override;
    int DoImport(const TCHAR* name, ImpInterface* i, Interface* gi, BOOL suppressPrompts = FALSE) override;

private:
    bool read(std::wstring_view fileName);

    bool createMeshObject(const Lib3MF::PWrapper& wrapper, 
                          const Lib3MF::PModel& model, 
                          const Lib3MF::PMeshObject& mesh, 
                          const sLib3MFTransform& transform, 
                          std::string_view objectName);

private:
    ImpInterface* _impInterface;
};

class ThreeMFImportClassDesc : public ClassDesc2
{
public:
    int IsPublic() override
    {
        return TRUE;
    }
    void* Create(BOOL) override
    {
        return new ThreeMFImport();
    }
    const TCHAR* ClassName() override
    {
        return _className;
    }
    SClass_ID SuperClassID() override
    {
        return SCENE_IMPORT_CLASS_ID;
    }
    Class_ID ClassID() override
    {
        return ThreeMFImport_CLASS_ID;
    }
    const TCHAR* Category() override
    {
        return NULL;
    }
    const TCHAR* InternalName() override
    {
        return _className;
    }
    HINSTANCE HInstance() override
    {
        return hInstance;
    }
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

#endif // PLUGIN_3DSMAX_THREEMFIMPORT_H
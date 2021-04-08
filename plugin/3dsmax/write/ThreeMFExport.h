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

#ifndef PLUGIN_3DSMAX_THREEMFEXPORT_H
#define PLUGIN_3DSMAX_THREEMFEXPORT_H

#include "resource.h"
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <maxtypes.h>

#include <impexp.h>

#include <lib3mf_implicit.hpp>

#include <string_view>

#define ThreeMFExport_CLASS_ID Class_ID(0x776d5450, 0x7fc77efd)

static const TCHAR _className[] = _T("ThreeMFExport");

extern HINSTANCE hInstance;

namespace M3mf {

class ThreeMFExport : public SceneExport
{
public:
    ThreeMFExport();
    virtual ~ThreeMFExport();

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
    BOOL SupportsOptions(int ext, DWORD options) override;
    int DoExport(const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts = FALSE, DWORD options = 0) override;

private:
    bool write(Interface* ip, INode* pNode, std::wstring_view filename, int iTreeDepth = 0);

    bool exportSelected(Interface* ip, const Lib3MF::PWrapper& wrapper, const Lib3MF::PModel& model);

    bool exportAll(INode* pNode, const Lib3MF::PWrapper& wrapper, const Lib3MF::PModel& model);

    bool createBuildItem(INode* childNode, const Lib3MF::PWrapper& wrapper, const Lib3MF::PModel& model);

private:
    bool _isSelected { false };
};

class ThreeMFExportClassDesc : public ClassDesc2
{
public:
    int IsPublic() override
    {
        return TRUE;
    }
    void* Create(BOOL /*loading = FALSE*/) override
    {
        return new ThreeMFExport();
    }
    const TCHAR* ClassName() override
    {
        return _className;
    }
    SClass_ID SuperClassID() override
    {
        return SCENE_EXPORT_CLASS_ID;
    }
    Class_ID ClassID() override
    {
        return ThreeMFExport_CLASS_ID;
    }
    const TCHAR* Category() override
    {
        return _className;
    }
    const TCHAR* InternalName() override
    {
        return _T("ThreeMFExport");
    }
    HINSTANCE HInstance() override
    {
        return hInstance;
    }
};

} // namespace M3mf

#endif // PLUGIN_3DSMAX_THREEMFIMPORT_H

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

#include "ThreeMFExport.h"
#include "types.h"
#include "utility.h"

#include <Max.h>

#include <stdmat.h>

ClassDesc2* GetThreeMFExportDesc()
{
    static M3mf::ThreeMFExportClassDesc threeMfClassDesc;
    return &threeMfClassDesc;
}

namespace M3mf {

ThreeMFExport::ThreeMFExport()
{
}

ThreeMFExport::~ThreeMFExport()
{
}

int ThreeMFExport::ExtCount()
{
    return 1;
}

const TCHAR* ThreeMFExport::Ext(int n)
{
    switch (n) {
    case 0:
        return _T("3mf");
    default:
        return nullptr;
    }

    return nullptr;
}

const TCHAR* ThreeMFExport::LongDesc()
{
    return _T("3MF Export for 3DSMax");
}

const TCHAR* ThreeMFExport::ShortDesc()
{
    return _T("3MF Export");
}

const TCHAR* ThreeMFExport::AuthorName()
{
    return _T("Hamed Sabri");
}

const TCHAR* ThreeMFExport::CopyrightMessage()
{
    return _T("Copyright 2021 Hamed Sabri");
}

const TCHAR* ThreeMFExport::OtherMessage1()
{
    return _T("");
}

const TCHAR* ThreeMFExport::OtherMessage2()
{
    return _T("");
}

unsigned int ThreeMFExport::Version()
{
    return 100;
}

void ThreeMFExport::ShowAbout(HWND /*hWnd*/)
{
}

BOOL ThreeMFExport::SupportsOptions(int /*ext*/, DWORD /*options*/)
{
    return TRUE;
}

int ThreeMFExport::DoExport(const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts, DWORD options)
{
    _isSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;

    INode* pRootNode = ip->GetRootNode();

    std::wstring_view fName(name);

    return write(ip, pRootNode, fName);
}

bool ThreeMFExport::write(Interface* ip, INode* pNode, std::wstring_view filename, int iTreeDepth)
{
    Lib3MF::PWrapper wrapper = Lib3MF::CWrapper::loadLibrary();
    Lib3MF::PModel model = wrapper->CreateModel();

    if (_isSelected) {
        if (!exportSelected(ip, wrapper, model)) {
            return false;
        }
    } else {
        if (!exportAll(pNode, wrapper, model)) {
            return false;
        }
    }

    Lib3MF::PWriter writer = model->QueryWriter("3mf");
    try {
        writer->WriteToFile(wstring_to_utf8(filename.data()));
    } catch (Lib3MF::ELib3MFException e) {
        return false;
    }

    return true;
}

bool ThreeMFExport::exportSelected(Interface* ip, const Lib3MF::PWrapper& wrapper, const Lib3MF::PModel& model)
{
    for (auto i = 0; i < ip->GetSelNodeCount(); i++) {
        INode* childNode = ip->GetSelNode(i);
        createBuildItem(childNode, wrapper, model);
    }

    return true;
}

bool ThreeMFExport::exportAll(INode* pNode, const Lib3MF::PWrapper& wrapper, const Lib3MF::PModel& model)
{
    for (auto i = 0; i < pNode->NumberOfChildren(); i++) {
        INode* childNode = pNode->GetChildNode(i);
        createBuildItem(childNode, wrapper, model);
    }

    return true;
}

bool ThreeMFExport::createBuildItem(INode* childNode, const Lib3MF::PWrapper& wrapper, const Lib3MF::PModel& model)
{
    Object* pObject = childNode->GetObjectRef();

    // check if the object can be converted into an edtiable mesh
    if (pObject != nullptr && pObject->CanConvertToType(triObjectClassID)) {
        TriObject* pTriObject = static_cast<TriObject*>(pObject);
        pTriObject->ConvertToType(0, triObjectClassID);

        Mesh* pMesh = &pTriObject->GetMesh();

        // vertices
        std::vector<Lib3MF::sPosition> vertices;
        vertices.resize(pMesh->getNumVerts());
        for (auto i = 0; i < vertices.size(); ++i) {
            Point3* vertPos = &pMesh->getVert(i);
            vertices[i].m_Coordinates[0] = static_cast<float>(vertPos->x);
            vertices[i].m_Coordinates[1] = static_cast<float>(vertPos->y);
            vertices[i].m_Coordinates[2] = static_cast<float>(vertPos->z);
        }

        // triangle
        std::vector<Lib3MF::sTriangle> triangles;
        triangles.resize(pMesh->getNumFaces());
        for (auto i = 0; i < triangles.size(); ++i) {
            Face* face = &pMesh->faces[i];
            DWORD* pIndices = face->getAllVerts();
            for (auto j = 0; j < 3; ++j) {
                triangles[i].m_Indices[j] = pIndices[j];
            }
        }

        // add mesh object
        auto meshObject = model->AddMeshObject();
        std::wstring_view wfileName(childNode->GetName());
        meshObject->SetName(wstring_to_utf8(wfileName.data()));
        meshObject->SetGeometry(vertices, triangles);

        // get the local transform
        Matrix3 worldTM = childNode->GetNodeTM(0);
        Matrix3 parentTM = childNode->GetParentTM(0);
        Matrix3 localTM = worldTM * Inverse(parentTM);

        // Material
        M3mf::ColorM diffuseColorM { 0.5f, 0.5f, 0.5f };

        Mtl* mtl = childNode->GetMtl();

        StdMat2* stdmat = dynamic_cast<StdMat2*>(mtl);
        if (stdmat) {
            auto diffColor = stdmat->GetDiffuse(0);
            diffuseColorM[0] = diffColor.r;
            diffuseColorM[1] = diffColor.g;
            diffuseColorM[2] = diffColor.b;
        }

        auto baseMaterialGroup = model->AddBaseMaterialGroup();
        Lib3MF_uint32 color = baseMaterialGroup->AddMaterial("Material Color", wrapper->FloatRGBAToColor(diffuseColorM[0], diffuseColorM[1], diffuseColorM[2], 1.0f));

        sLib3MFTriangleProperties sTriangleProperty;
        sTriangleProperty.m_ResourceID = baseMaterialGroup->GetResourceID();
        sTriangleProperty.m_PropertyIDs[0] = color;
        sTriangleProperty.m_PropertyIDs[1] = color;
        sTriangleProperty.m_PropertyIDs[2] = color;

        for (auto i = 0; i < pMesh->getNumFaces(); i++) {
            meshObject->SetTriangleProperties(i, sTriangleProperty);
        }

        // set object property
        meshObject->SetObjectLevelProperty(sTriangleProperty.m_ResourceID, sTriangleProperty.m_PropertyIDs[0]);

        model->AddBuildItem(meshObject.get(), M3mf::convert(localTM));
    }

    return true;
}

} // namespace M3mf

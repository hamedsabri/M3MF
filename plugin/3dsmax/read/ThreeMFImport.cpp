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

#include "ThreeMFImport.h"
#include "utility.h"
#include "types.h"

#include <Max.h>
#include <color.h>

#include <stdmat.h>

ClassDesc2* GetThreeMFImportDesc()
{
    static M3mf::ThreeMFImportClassDesc threeMfClassDesc;
    return &threeMfClassDesc;
}

namespace M3mf {

ThreeMFImport::ThreeMFImport()
{
}

ThreeMFImport::~ThreeMFImport()
{
}

int ThreeMFImport::ExtCount()
{
    return 1;
}

const TCHAR* ThreeMFImport::Ext(int n)
{
    switch (n) {
    case 0:
        return _T("3mf");
    default:
        return nullptr;
    }

    return nullptr;
}

const TCHAR* ThreeMFImport::LongDesc()
{
    return _T("3MF Import for 3DSMax");
}

const TCHAR* ThreeMFImport::ShortDesc()
{
    return _T("3MF Import");
}

const TCHAR* ThreeMFImport::AuthorName()
{
    return _T("Hamed Sabri");
}

const TCHAR* ThreeMFImport::CopyrightMessage()
{
    return _T("Copyright 2021 Hamed Sabri");
}

const TCHAR* ThreeMFImport::OtherMessage1()
{
    return _T("");
}

const TCHAR* ThreeMFImport::OtherMessage2()
{
    return _T("");
}

unsigned int ThreeMFImport::Version()
{
    return 100;
}

void ThreeMFImport::ShowAbout(HWND /*hWnd*/)
{
}

int ThreeMFImport::DoImport(const TCHAR* filename, ImpInterface* importerInt, Interface* ip, BOOL suppressPrompts)
{
    _impInterface = importerInt;

    std::wstring_view fName(filename);
    read(fName);

    return TRUE;
}

bool ThreeMFImport::read(std::wstring_view fileName)
{
    // read the 3mf file
    Lib3MF::PWrapper wrapper = Lib3MF::CWrapper::loadLibrary();
    Lib3MF::PModel model = wrapper->CreateModel();
    Lib3MF::PReader reader3MF = model->QueryReader("3mf");
    try {
        reader3MF->ReadFromFile(wstring_to_utf8(fileName.data()));
    } catch (Lib3MF::ELib3MFException e) {
        return e.getErrorCode();
    }

    // iterate through builditem(s)
    Lib3MF::PBuildItemIterator buildItemIterator = model->GetBuildItems();
    while (buildItemIterator->MoveNext()) {
        Lib3MF::PBuildItem buildItem = buildItemIterator->GetCurrent();
        Lib3MF::PObject object = buildItem->GetObjectResource();

        // name
        std::string_view objectName;
        if (object->GetName().empty()) {
            const std::string name { "Object_" + std::to_string(buildItem->GetObjectResourceID()) };
            objectName = name;
        } else {
            objectName = object->GetName();
        }

        // transform
        sLib3MFTransform affineTransform;
        if (buildItem->HasObjectTransform()) {
            affineTransform = std::move(buildItem->GetObjectTransform());
        } else {
            affineTransform = std::move(wrapper->GetIdentityTransform());
        }

        // components
        if (object->IsComponentsObject()) {
            Lib3MF::PComponentsObject componentsObject = model->GetComponentsObjectByID(object->GetResourceID());

            // we care only about model. ignore support, solidsupport, other
            if (componentsObject->GetType() == Lib3MF::eObjectType::Model) {
                for (uint32_t nIndex = 0; nIndex < componentsObject->GetComponentCount(); nIndex++) {
                    Lib3MF::PComponent component = componentsObject->GetComponent(nIndex);

                    sLib3MFTransform compAffineTransform;
                    if (component->HasTransform()) {
                        compAffineTransform = std::move(component->GetTransform());
                    } else {
                        compAffineTransform = std::move(affineTransform);
                    }

                    Lib3MF::PMeshObject meshObject = model->GetMeshObjectByID(component->GetObjectResourceID());
                    bool status = createMeshObject(wrapper, model, meshObject, compAffineTransform, objectName);
                    if (!status) {
                        return false;
                    }
                }
            }
        }

        // mesh
        if (object->IsMeshObject()) {
            Lib3MF::PMeshObject meshObject = model->GetMeshObjectByID(object->GetResourceID());
            bool status = createMeshObject(wrapper, model, meshObject, affineTransform, objectName);

            if (!status) {
                return false;
            }
        }
    }

    return true;
}

bool ThreeMFImport::createMeshObject(const Lib3MF::PWrapper& wrapper, const Lib3MF::PModel& model, const Lib3MF::PMeshObject& mesh, const sLib3MFTransform& transform, std::string_view objectName)
{
    bool status { true };

    // create a new TriObject
    TriObject* object = CreateNewTriObject();
    if (!object) {
        return false;
    }

    // get the pointer to the Mesh
    Mesh* mMesh = &object->GetMesh();

    // set vertex positions for the Mesh
    std::vector<Lib3MF::sPosition> vertPos;
    mesh->GetVertices(vertPos);

    // base material color
    M3mf::ColorM materialColor { 0.5f, 0.5f, 0.5f };

    status = mMesh->setNumVerts(mesh->GetVertexCount());
    if (!status) {
        return false;
    }
    for (auto index = 0; index < vertPos.size(); ++index) {
        mMesh->setVert(index, vertPos[index].m_Coordinates[0], vertPos[index].m_Coordinates[1], vertPos[index].m_Coordinates[2]);
    }

    // set triangle indices for the Mesh
    std::vector<Lib3MF::sTriangle> triangleIndices;
    mesh->GetTriangleIndices(triangleIndices);

    status = mMesh->setNumFaces(mesh->GetTriangleCount());
    if (!status) {
        return false;
    }

    // triangle properties
    std::vector<Lib3MF::sTriangleProperties> properties;
    mesh->GetAllTriangleProperties(properties);

    // verify that lib3mf resource ID is present in model
    ResourceIDCheck resourceIDCheck(model);

    for (uint32_t index = 0; index < mesh->GetTriangleCount(); ++index) {
        // create a face and set it's indicies
        Face& face = mMesh->faces[index];
        face.setMatID(1);
        face.setEdgeVisFlags(1, 1, 1);

        // face indices
        DWORD indicies[3];
        indicies[0] = triangleIndices[index].m_Indices[0];
        indicies[1] = triangleIndices[index].m_Indices[1];
        indicies[2] = triangleIndices[index].m_Indices[2];

        face.setVerts(indicies);

        // BaseMaterial
        if (resourceIDCheck.isRessourceIDValid(properties[index].m_ResourceID) && model->GetPropertyTypeByID(properties[index].m_ResourceID) == Lib3MF::ePropertyType::BaseMaterial) {
            Lib3MF::PBaseMaterialGroup baseMaterialGroup = model->GetBaseMaterialGroupByID(properties[index].m_ResourceID);

            sLib3MFColor color(baseMaterialGroup->GetDisplayColor(properties[index].m_PropertyIDs[0]));

            wrapper->ColorToFloatRGBA(color, materialColor[0], materialColor[1], materialColor[2], materialColor[3]);
        }
    }

    // build bbox and invalidate cache
    mMesh->buildBoundingBox();
    mMesh->InvalidateGeomCache();
    mMesh->InvalidateTopologyCache();

    // create a scenegraph node
    ImpNode* node = _impInterface->CreateNode();
    if (!node) {
        delete object;
        return false;
    }

    // set the affine matrix
    Matrix3 xformM(M3mf::convert(transform));
    node->SetTransform(0, xformM);

    // set the reference
    node->Reference(object);

    // add the node to the scene
    _impInterface->AddNodeToScene(node);

    // create a new Standard material
    StdMat2* standardMat = NewDefaultStdMat();
    standardMat->SetName(_T("Standard Material"));
    standardMat->SetAmbient(Color(materialColor[0], materialColor[1], materialColor[2]), 0);
    standardMat->SetDiffuse(Color(materialColor[0], materialColor[1], materialColor[2]), 0);
    standardMat->SetSpecular(Color(materialColor[0], materialColor[1], materialColor[2]), 0);

    // assign the material to the node
    node->GetINode()->SetMtl(standardMat);

    // draw update
    _impInterface->RedrawViews();

    return status;
}

ResourceIDCheck::ResourceIDCheck(Lib3MF::PModel model)
{
    auto pResources = model->GetResources();
    if (pResources->Count() != 0) {
        while (pResources->MoveNext()) {
            auto pResource = pResources->GetCurrent();
            _resourceIDs.insert(pResource->GetResourceID());
        }
    }
}

bool ResourceIDCheck::isRessourceIDValid(uint32_t ID)
{
    auto iter = _resourceIDs.find(ID);
    if (iter != _resourceIDs.end()) {
        return true;
    } else {
        return false;
    }
}

} // namespace M3mf
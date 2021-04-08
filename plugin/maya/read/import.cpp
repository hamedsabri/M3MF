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

#include "import.h"
#include "utility.h"

#include <maya/MDagPath.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSet.h>
#include <maya/MFnTransform.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MPointArray.h>
#include <maya/MSelectionList.h>

#include <vector>
#include <assert.h>

namespace M3mf {

bool Import::read(std::string_view fileName)
{
    // read the 3mf file
    Lib3MF::PWrapper wrapper = Lib3MF::CWrapper::loadLibrary();
    Lib3MF::PModel model = wrapper->CreateModel();
    Lib3MF::PReader reader3MF = model->QueryReader("3mf");
    try {
        reader3MF->ReadFromFile(fileName.data());
    } catch (Lib3MF::ELib3MFException e) {
        MGlobal::displayError(e.what());
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
                    MStatus status = createMeshObject(wrapper, model, meshObject, compAffineTransform, objectName);
                    if (!status) {
                        return false;
                    }
                }
            }
        }

        // mesh
        if (object->IsMeshObject()) {
            Lib3MF::PMeshObject meshObject = model->GetMeshObjectByID(object->GetResourceID());
            MStatus status = createMeshObject(wrapper, model, meshObject, affineTransform, objectName);

            if (!status) {
                return false;
            }
        }
    }

    return true;
}

MStatus Import::createMeshObject(const Lib3MF::PWrapper& wrapper, const Lib3MF::PModel& model, const Lib3MF::PMeshObject& mesh, const sLib3MFTransform& transform, std::string_view objectName)
{
    MStatus status { MS::kSuccess };

    MPointArray vertPosArray(mesh->GetVertexCount());
    MIntArray faceCounts(mesh->GetTriangleCount());
    MIntArray faceIndices;

    // vertex index, color(rgba)
    M3mf::SetOfIndexAndVertColor vertColorSet;

    // base material color
    M3mf::Color materialColor { 0.5f, 0.5f, 0.5f };

    {
        // fill out verts positions
        std::vector<Lib3MF::sPosition> vertPos;
        mesh->GetVertices(vertPos);

        for (uint32_t index = 0; index < vertPos.size(); ++index) {
            vertPosArray.set(index, vertPos[index].m_Coordinates[0], vertPos[index].m_Coordinates[1], vertPos[index].m_Coordinates[2]);
        }

        // fill out polygonCounts, polygonConnects
        std::vector<Lib3MF::sTriangle> triangleIndices;
        mesh->GetTriangleIndices(triangleIndices);

        // triangle properties
        std::vector<Lib3MF::sTriangleProperties> properties;
        mesh->GetAllTriangleProperties(properties);

        // verify that lib3mf resource ID is present in model
        ResourceIDCheck resourceIDCheck(model);

        for (uint32_t index = 0; index < mesh->GetTriangleCount(); ++index) {
            // each face is a triangle only.
            faceCounts.set(3, index);

            // face indices
            for (int tIndex = 0; tIndex < 3; ++tIndex) {
                faceIndices.append(triangleIndices[index].m_Indices[tIndex]);
            }

            // color
            if (resourceIDCheck.isRessourceIDValid(properties[index].m_ResourceID) && model->GetPropertyTypeByID(properties[index].m_ResourceID) == Lib3MF::ePropertyType::Colors) {
                Lib3MF::PColorGroup colorGroup = model->GetColorGroupByID(properties[index].m_ResourceID);
                for (int j = 0; j < 3; j++) {
                    M3mf::Color vertexColor;
                    wrapper->ColorToFloatRGBA(colorGroup->GetColor(properties[index].m_PropertyIDs[j]), vertexColor[0], vertexColor[1], vertexColor[2], vertexColor[3]);

                    vertColorSet.emplace(std::make_pair(triangleIndices[index].m_Indices[j], vertexColor));
                }
            }

            // baseMaterial
            if (resourceIDCheck.isRessourceIDValid(properties[index].m_ResourceID) && model->GetPropertyTypeByID(properties[index].m_ResourceID) == Lib3MF::ePropertyType::BaseMaterial) {
                Lib3MF::PBaseMaterialGroup baseMaterialGroup = model->GetBaseMaterialGroupByID(properties[index].m_ResourceID);

                sLib3MFColor color(baseMaterialGroup->GetDisplayColor(properties[index].m_PropertyIDs[0]));

                wrapper->ColorToFloatRGBA(color, materialColor[0], materialColor[1], materialColor[2], materialColor[3]);
            }
        }
    }

    // dummy transform
    MFnTransform fnTransform;
    MObject parent = fnTransform.create(MObject::kNullObj, &status);
    if (!status) {
        M3mf::messageBox("Error", "Failed to create dummy transform!");
        return status;
    }

    // mesh object
    MFnMesh meshFn;
    MObject meshObject = meshFn.create(vertPosArray.length(), faceCounts.length(), vertPosArray, faceCounts, faceIndices, parent, &status);

    if (status != MS::kSuccess) {
        M3mf::messageBox("Error", "Failed to create MFnMesh!");
        return status;
    }

    // set affine transform
    MTransformationMatrix xformM(M3mf::convert(transform));
    fnTransform.set(xformM.asMatrix());

    // vertex color
    if (vertColorSet.size() > 0) {
        status = assignVertexColors(meshObject, vertColorSet);
    }

    // base material
    status = assignLamabertShader(meshObject, objectName.data(), MColor(materialColor[0], materialColor[1], materialColor[2], materialColor[3]));

    return status;
}

MStatus Import::assignVertexColors(const MObject& object, const SetOfIndexAndVertColor& vertColorSet)
{
    MStatus status { MS::kSuccess };

    if (object.apiType() != MFn::kMesh) {
        return MS::kFailure;
    }

    MFnMesh meshFn(object);

    // setVertexColors
    const uint32_t size = static_cast<uint32_t>(vertColorSet.size());
    MColorArray colorArray(size);
    MIntArray vertexList(size);

    for (auto const& [index, color] : vertColorSet) {
        vertexList.append(index);
        MColor mColor(color[0], color[1], color[2], color[3]);
        colorArray.append(mColor);
    }

    assert(colorArray.length() == vertexList.length());

    status = meshFn.setVertexColors(colorArray, vertexList);
    if (!status) {
        M3mf::messageBox("Error", "Failed to setVertexColors!");
        return MS::kFailure;
    }

    MPlug plg = meshFn.findPlug("displayColors");
    if (!plg.isNull()) {
        plg.setBool(true);
    }

    return status;
}

MStatus Import::assignLamabertShader(const MObject& object, std::string_view shaderName, const MColor& color)
{
    MStatus status { MS::kSuccess };

    MFnLambertShader lambertFn;
    MObject lambertObject = lambertFn.create();
    lambertFn.setColor(color);
    lambertFn.setDiffuseCoeff(1.0);

    // rename
    MDGModifier dagMod;
    dagMod.renameNode(lambertObject, shaderName.data());
    dagMod.doIt();

    MPlug outColorPlug = lambertFn.findPlug("outColor", &status);
    if (!status) {
        M3mf::messageBox("Error", "finding outColor plug failed!");
        return status;
    }

    MFnSet fnSet;
    MSelectionList selList;
    MObject shadingEngine = fnSet.create(selList, MFnSet::kRenderableOnly, &status);
    if (!status) {
        M3mf::messageBox("Error", "creating shadingEngine failed!");
        return status;
    }

    fnSet.setName("shadingEngineGroup", &status);
    fnSet.addMember(object);
    MFnSet fnSetShaderPlg(shadingEngine, &status);
    if (!status) {
        M3mf::messageBox("Error", "shadingEngineGroup failed!");
        return status;
    }

    MPlug surfaceShaderPlug = fnSetShaderPlg.findPlug("surfaceShader", &status);
    if (!status) {
        M3mf::messageBox("Error", "Finding surfaceShader plug failed!");
        return status;
    }

    connect(outColorPlug, surfaceShaderPlug, true);

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

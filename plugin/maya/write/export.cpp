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

#include "export.h"
#include "types.h"
#include "utility.h"

#include <maya/MDagPath.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnTransform.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MSelectionList.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MVector.h>

#include <vector>

namespace M3mf {

bool Export::write(std::string_view fileName, bool isSelected)
{
    Lib3MF::PWrapper wrapper = Lib3MF::CWrapper::loadLibrary();
    Lib3MF::PModel model = wrapper->CreateModel();

    // set unit system
    model->SetUnit(M3mf::convertMayaToModelUnit());

    if (isSelected) {
        if (!exportSelected(wrapper, model)) {
            return false;
        }
    } else {
        if (!exportAll(wrapper, model)) {
            return false;
        }
    }

    Lib3MF::PWriter writer = model->QueryWriter("3mf");
    try {
        writer->WriteToFile(fileName.data());
    } catch (Lib3MF::ELib3MFException e) {
        MGlobal::displayError(e.what());
        return false;
    }

    return true;
}

bool Export::exportSelected(const Lib3MF::PWrapper& wrapper, const Lib3MF::PModel& model)
{
    MSelectionList list;
    MGlobal::getActiveSelectionList(list);
    if (list.length() == 0) {
        M3mf::messageBox("Error", "selection list is empty.");
        return false;
    }

    // iterate through all selected items
    MStatus status { MS::kSuccess };
    for (uint32_t i = 0; i < list.length(); ++i) {
        MDagPath path;
        list.getDagPath(i, path);

        if (path.node().apiType() == MFn::kMesh) {
            MFnDagNode dagFn(path);
            dagFn.getPath(path);
            path.pop(); // pop from the shape to the transform
        }

        // create build item(s)
        if (!createBuildItem(path, wrapper, model)) {
            return false;
        }
    }

    return true;
}

bool Export::exportAll(const Lib3MF::PWrapper& wrapper, const Lib3MF::PModel& model)
{
    MStatus status { MS::kSuccess };

    MItDependencyNodes it(MFn::kMesh);
    while (!it.isDone()) {
        MObject obj = it.item();
        MDagPath pathToMeshObject = MDagPath::getAPathTo(obj);

        MFnDagNode dagFn(pathToMeshObject);
        MDagPath path;
        dagFn.getPath(path);
        path.pop(); // pop from the shape to the transform

        // create build item(s)
        if (!createBuildItem(path, wrapper, model)) {
            return false;
        }

        it.next();
    }

    return true;
}

bool Export::createBuildItem(MDagPath& dagPath, const Lib3MF::PWrapper& wrapper, const Lib3MF::PModel& model)
{
    MStatus status { MS::kSuccess };

    // xform
    MFnTransform xform(dagPath);
    MTransformationMatrix xformM = xform.transformation(&status);
    if (!status) {
        M3mf::messageBox("Error", "Failed to get MTransformationMatrix.");
        return false;
    }

    // mesh
    if (dagPath.apiType() == MFn::kTransform) {
        dagPath.extendToShape();
        if (dagPath.node().apiType() != MFn::kMesh) {
            M3mf::messageBox("Error", "Object is not a mesh.");
            return false;
        }
    }

    MFnMesh meshFn(dagPath.node(), &status);
    if (!status) {
        M3mf::messageBox("Error", "Failed to create MFnMesh.");
        return false;
    }

    // vertices
    MFloatPointArray vertPositions;
    meshFn.getPoints(vertPositions);

    std::vector<Lib3MF::sPosition> vertices;
    vertices.resize(vertPositions.length());
    for (auto i = 0; i < vertices.size(); ++i) {
        MVector vertPos(vertPositions[i].x, vertPositions[i].y, vertPositions[i].z);
        vertices[i].m_Coordinates[0] = static_cast<float>(vertPos.x);
        vertices[i].m_Coordinates[1] = static_cast<float>(vertPos.y);
        vertices[i].m_Coordinates[2] = static_cast<float>(vertPos.z);
    }

    // triangle
    std::vector<Lib3MF::sTriangle> triangles;
    triangles.resize(meshFn.numPolygons());
    MIntArray faceVertices;
    for (auto i = 0; i < meshFn.numPolygons(); ++i) {
        meshFn.getPolygonVertices(i, faceVertices);
        if (faceVertices.length() < 3) {
            continue;
        }
        if (faceVertices.length() > 3) {
            m_needToTriangulate = true;
            break;
        }
        for (auto j = 0; j < 3; ++j) {
            triangles[i].m_Indices[j] = faceVertices[j];
        }
    }

    if (m_needToTriangulate) {
        M3mf::messageBox("Error", "Export failed. Please triangluate the mesh before export.");
        return false;
    }

    // add mesh object
    auto meshObject = model->AddMeshObject();
    meshObject->SetName(meshFn.fullPathName().asChar());
    meshObject->SetGeometry(vertices, triangles);

    // color group
    MColorArray vertColors;
    meshFn.getFaceVertexColors(vertColors);

    if (vertColors.length() > 0) {
        auto colorGroup = model->AddColorGroup();
        for (auto i = 0; i < meshFn.numPolygons(); i++) {
            sLib3MFTriangleProperties sTriangleProperty;
            sTriangleProperty.m_ResourceID = colorGroup->GetResourceID();
            for (int j = 0; j < 3; j++) {
                sTriangleProperty.m_PropertyIDs[j] = colorGroup->AddColor(wrapper->FloatRGBAToColor(vertColors[i * 3 + j][0], vertColors[i * 3 + j][1], vertColors[i * 3 + j][2], 1.0f));
            }

            meshObject->SetTriangleProperties(i, sTriangleProperty);

            // Object Level Property
            if (i == 0) {
                meshObject->SetObjectLevelProperty(sTriangleProperty.m_ResourceID, sTriangleProperty.m_PropertyIDs[0]);
            }
        }
    } else {
        // Material
        MObjectArray shaders;
        MIntArray indices;
        meshFn.getConnectedShaders(0, shaders, indices);
        M3mf::Color diffuseColor;
        for (uint32_t index = 0; index < shaders.length(); ++index) {
            MPlugArray connections;
            MFnDependencyNode shaderGroup(shaders[index]);
            MPlug shaderPlug = shaderGroup.findPlug("surfaceShader");

            if (!shaderPlug.isNull()) {
                shaderPlug.connectedTo(connections, true, false);
                for (uint32_t j = 0; j < connections.length(); ++j) {
                    MObject shaderNode = connections[j].node();
                    if (shaderNode != MObject::kNullObj) {
                        MPlug colorPlug = MFnDependencyNode(shaderNode).findPlug("color", &status);
                        if (status != MS::kFailure) {
                            MObject data;
                            colorPlug.getValue(data);
                            MFnNumericData val(data);
                            val.getData(diffuseColor[0], diffuseColor[1], diffuseColor[2]);
                        }
                    }
                }
            }
        }

        auto baseMaterialGroup = model->AddBaseMaterialGroup();
        Lib3MF_uint32 diffColor = baseMaterialGroup->AddMaterial("Material Color", wrapper->FloatRGBAToColor(diffuseColor[0], diffuseColor[1], diffuseColor[2], 1.0f));

        sLib3MFTriangleProperties sTriangleProperty;
        sTriangleProperty.m_ResourceID = baseMaterialGroup->GetResourceID();
        sTriangleProperty.m_PropertyIDs[0] = diffColor;
        sTriangleProperty.m_PropertyIDs[1] = diffColor;
        sTriangleProperty.m_PropertyIDs[2] = diffColor;

        for (auto i = 0; i < meshFn.numPolygons(); i++) {
            meshObject->SetTriangleProperties(i, sTriangleProperty);
        }

        // set object property
        meshObject->SetObjectLevelProperty(sTriangleProperty.m_ResourceID, sTriangleProperty.m_PropertyIDs[0]);
    }

    model->AddBuildItem(meshObject.get(), M3mf::convert(xformM.asMatrix()));

    return true;
}

} // namespace M3mf
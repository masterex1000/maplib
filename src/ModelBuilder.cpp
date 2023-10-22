#include "raymath.h"
#include "mapflib/qma/qmap.h"
#include "types.h"

size_t maptool::lossyHash(Vector2 vector) {
    size_t h1 = std::hash<uint64_t>{}( (uint64_t)(vector.x * 1000) );
    size_t h2 = std::hash<uint64_t>{}( (uint64_t)(vector.y * 1000) );
    return h1 ^ ( h2 << 1);
}

size_t maptool::lossyHash(Vector3 vector) {
    size_t h1 = std::hash<uint64_t>{}( (uint64_t)(vector.x * 1000) );
    size_t h2 = std::hash<uint64_t>{}( (uint64_t)(vector.y * 1000) );
    size_t h3 = std::hash<uint64_t>{}( (uint64_t)(vector.z * 1000) );
    return h1 ^ ( (h2 ^ (h3 << 1) ) << 1);
}

void maptool::ModelBuilder::setObject(std::string objectName) {
    if(currentObjName == objectName)
        return;
    
    currentObjName = objectName;
    currentObject = NULL;
    currentMaterialGroup = NULL; // Invalidate because this depends on object
}

void maptool::ModelBuilder::setMaterial(std::string materialName) {
    if(currentMatName == materialName)
        return;
    
    currentMatName = materialName;
    currentMaterialGroup = NULL;
}

void maptool::ModelBuilder::updateCurrentObject() {
    const auto sameObjName = [this](const MeshObject &o) {
        return (this->currentObjName == o.name);
    };

    if(auto it = std::find_if(model.objects.begin(), model.objects.end(), sameObjName); it != std::end(model.objects)) {
        currentObject = &(*it);
    } else {
        MeshObject o(currentObjName);
        model.objects.push_back(o);
        currentObject = &(model.objects.back());
    }
}

void maptool::ModelBuilder::updateCurrentMaterial() {
    const auto sameMatName = [this](const MaterialGroup &m) {
        return (this->currentMatName == m.materialName);
    };

    if(auto it = std::find_if(currentObject->subMeshes.begin(), currentObject->subMeshes.end(), sameMatName); it != std::end(currentObject->subMeshes)) {
        currentMaterialGroup = &(*it);
    } else {
        MaterialGroup g(currentMatName);
        currentObject->subMeshes.push_back(g);
        currentMaterialGroup = &(currentObject->subMeshes.back());
    }
}

void maptool::ModelBuilder::addVertex(Vector3 pos, Vector2 tex, Vector3 normal) {
    if(currentObject == NULL)
        updateCurrentObject();

    if(currentMaterialGroup == NULL)
        updateCurrentMaterial();

    // Check if pos dup already exists. If so use its id, else add new vertex
    int posIndex  = maptool::addVertexToTables(posLookup, model.vertices, pos);
    int texIndex  = maptool::addVertexToTables(texLookup, model.textureVerts, tex);
    int normIndex = maptool::addVertexToTables(normalLookup, model.normalVerts, normal);

    // Add the ver indicies to currentMaterialGroup

    currentMaterialGroup->coords.push_back({posIndex, texIndex, normIndex});
}
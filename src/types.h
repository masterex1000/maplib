#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace maptool {

struct MaterialGroup {
    std::string materialName;
    std::vector<std::array<int, 3>> coords; // pos, texture, normal

    MaterialGroup(std::string materialName) : materialName(materialName) {}
    MaterialGroup() = default;
};

struct MeshObject {
    std::string name;
    std::vector<MaterialGroup> subMeshes;

    MeshObject(std::string name) : name(name) {}
    MeshObject() = default;
};

struct Model {
    std::vector<Vector3> vertices;
    std::vector<Vector2> textureVerts;
    std::vector<Vector3> normalVerts;

    std::vector<MeshObject> objects;
};

size_t lossyHash(Vector2 vector);
size_t lossyHash(Vector3 vector);

class ModelBuilder {
    Model model;

    std::string currentObjName = "";
    std::string currentMatName = "";

    MeshObject *currentObject = NULL;
    MaterialGroup *currentMaterialGroup = NULL;

    // These help lookup duplicate position, texture, and normal verticies
    std::map<std::size_t, int> posLookup;
    std::map<std::size_t, int> texLookup;
    std::map<std::size_t, int> normalLookup;

public:
    ModelBuilder() = default;

    void setObject(std::string objectName);
    void setMaterial(std::string material);
    void addVertex(Vector3 pos, Vector2 tex, Vector3 normal); // Every three calls is a triangle

    Model getModel() { return model; }

private:
    void updateCurrentObject();
    void updateCurrentMaterial();
};


inline bool vectorEquals(Vector3 a, Vector3 b) {
    return (bool) Vector3Equals(a, b);
}

inline bool vectorEquals(Vector2 a, Vector2 b) {
    return (bool) Vector2Equals(a, b);
}

template<typename t>
int findDuplicateVertex(const std::map<std::size_t, int> &lookupTable, const std::vector<t> &vertices, t &vertex) {
    size_t hash = lossyHash(vertex);

    auto it = lookupTable.find(hash);
    if(it == lookupTable.end())
        return -1;
    
    if(vectorEquals(vertices[it->second], vertex)) {
        return it->second;
    }

    return -1; // Found one that matches hash, but not same
}

template<typename t>
int addVertexToTables(std::map<std::size_t, int> &lookupTable, std::vector<t> &vertices, t &vertex) {
    int existing = findDuplicateVertex(lookupTable, vertices, vertex);

    if(existing != -1)
        return existing;
    
    size_t hash = lossyHash(vertex);

    int newVertexId = vertices.size();
    vertices.push_back(vertex);
    
    auto it = lookupTable.find(hash);
    if(it == lookupTable.end()) { // Only populate if it doesn't exist
        lookupTable[hash] = newVertexId;
    }

    return newVertexId;
}

}
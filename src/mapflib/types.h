#pragma once

#include <vector>
#include <map>
#include <string>
#include <unordered_set>
#include <unordered_map>

#define VECTOR_TYPE double

namespace MapFLib {

struct Vector3 {
    double x, y, z;
};

struct TextureAxis {
    double x, y, z, offset;
};

struct BrushFace {
    Vector3 planePoints[3];

    int imageId; // related to image in cache

    // Valve format (we don't care about quake tbh)
    TextureAxis axisU, axisV;

    VECTOR_TYPE rot;

    VECTOR_TYPE scaleX, scaleY;
};

struct Brush {
    std::vector<BrushFace> faces;
};

struct Entity {
    std::map<std::string, std::string> properties;

    std::vector<Brush> brushes;
};

struct MapFileData {
    std::vector<Entity> entities;

    std::vector<std::string> textureList;
    std::unordered_map<std::string, int> textureLookup;
};

// Map Polygon data

struct PolygonGeometry {
    std::vector<Vector3> verts;
};

struct BrushGeometry {
    std::vector<PolygonGeometry> polys;
};

struct EntityGeometry {
    std::vector<BrushGeometry> brushes;
};

struct MapGeometry {
    std::vector<EntityGeometry> entities;
};

} // namespace MapFLib
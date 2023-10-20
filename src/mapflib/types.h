#pragma once

#include <vector>
#include <map>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "linalg.h"

namespace MapFLib {

constexpr double CMP_EPSILON = 0.000001;

struct TextureAxis {
    double x, y, z, offset;
};

struct BrushFace {
    linalg::aliases::double3 planePoints[3];

    int imageId; // related to image in cache

    linalg::aliases::double3 normal;
    double dist;

    // Valve format (we don't care about quake tbh)
    TextureAxis axisU, axisV;

    double rot;

    double scaleX, scaleY;
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
    std::vector<linalg::aliases::double3> verts;
    std::vector
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
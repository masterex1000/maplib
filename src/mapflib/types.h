#pragma once

#include <vector>
#include <map>
#include <string>
#include <unordered_set>

#define VECTOR_TYPE double

namespace MapFLib {

struct Vector3 {
    double x, y, z;
};

struct Brush {
    Vector3 planePoints[3];

    std::string imageId; // related to image in cache

    // Valve format (we don't care about quake tbh)
    Vector3 axisU, axisV;

    VECTOR_TYPE scaleX, scaleY;
};

struct Entity {
    std::map<std::string, std::string> properties;

    std::vector<Brush> brushes;
};

struct MapFileData {
    std::vector<Entity> entities;

    std::unordered_set<std::string> textureList;
};

} // namespace MapFLib
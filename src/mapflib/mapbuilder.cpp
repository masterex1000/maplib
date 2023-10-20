#include "mapbuilder.h"

using namespace linalg::aliases;

int MapFLib::buildMapGeometry(MapFLib::MapGeometry &geo, MapFLib::MapFileData &map) {

    for(auto e : map.entities) {
        MapFLib::EntityGeometry entGeo;
        MapFLib::buildEntityGeometry(entGeo, map, e);
    }

    return 0;
}

int MapFLib::buildEntityGeometry(MapFLib::EntityGeometry &entityGeo, MapFLib::MapFileData &map, MapFLib::Entity &ent) {
    
    for(auto b : ent.brushes) {
        MapFLib::BrushGeometry brushGeo;
        MapFLib::buildBrushGeometry(brushGeo, map, b);
    }

    return 0;
}

bool getPlaneIntersection(double3 n1, double3 n2, double3 n3, double d1, double d2, double d3, double3 &p) {
    using namespace linalg;

    double denom = dot(n1, cross(n2, n3));

    if(denom < MapFLib::CMP_EPSILON)
        return false;

    p = -d1 * (cross(n2, n3)) - d2 * (cross(n3, n1)) - d3 * (cross(n1, n2)) / denom;

    return true;
}

template<typename Iterator>
bool vertexInHull(Iterator begin, Iterator end, double3 vertex) {
    for(auto i = begin; i != end; i++) {
        
    }
}

bool getPlaneIntersection(const MapFLib::BrushFace &a, const MapFLib::BrushFace &b, const MapFLib::BrushFace &c, double3 &out) {
    return getPlaneIntersection(a.normal, b.normal, c.normal, a.dist, b.dist, c.dist, out);
}

int MapFLib::buildBrushGeometry(MapFLib::BrushGeometry &brushGeo, const MapFLib::MapFileData &map, const MapFLib::Brush &brush) {
    for(int i = 0; i < brush.faces.size() - 2; i++) {
        for(int j = i; j < brush.faces.size() - 1; j++) {
            for (int k = j; k < brush.faces.size(); k++) {

                if( (i == j) || (i == k) || (j == k))
                    continue;
                
                double3 intersect;

                if(!getPlaneIntersection(brush.faces[i], brush.faces[j], brush.faces[k], intersect))
                    continue;
                
                bool legal = true;

                for(auto face : brush.faces) {
                    dot()
                }
            }
        }
    }
    
    return 0;
}

// int Map
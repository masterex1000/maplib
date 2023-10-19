#include "mapbuilder.h"

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

int MapFLib::buildBrushGeometry(MapFLib::BrushGeometry &brushGeo, const MapFLib::MapFileData &map, const MapFLib::Brush &brush) {
    return 0;
}

// int Map
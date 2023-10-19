#pragma once

#include "types.h"

namespace MapFLib {
    int buildMapGeometry(MapGeometry &geo, MapFileData &map);
    int buildEntityGeometry(MapFLib::EntityGeometry &entityGeo, MapFLib::MapFileData &map, MapFLib::Entity &ent);
    int buildBrushGeometry(MapFLib::BrushGeometry &brushGeo, const MapFLib::MapFileData &map, const MapFLib::Brush &brush);
}
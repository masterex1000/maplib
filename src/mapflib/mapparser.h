#pragma once

#include <istream>
#include <string>
#include <sstream>

#include "types.h"

namespace MapFLib {

    enum ParserState {
        PARSER_STATE_ROOT = 0, // What we're at at the start of the file
        PARSER_STATE_ENTITY, // Once we're in an entity
        PARSER_STATE_BRUSH  // When we are in a brush
    };

    int parseMapFile(MapFLib::MapFileData &map, std::istream& stream);

    bool parseEntity(std::istream &stream, MapFLib::Entity &entity);
    bool parseEntityProperty(std::istream &stream, std::string &key, std::string &value);
    bool parseEntityBrush(std::istream &stream, MapFLib::Brush &brush);
    bool praseVector3(std::istream &stream, MapFLib::Vector3 &vector);
    bool parseTextureAxis(std::istream &stream, MapFLib::Brush);
    bool parseComment(std::istream &stream); // Handles lines that only contain comment that starts with "//"
}
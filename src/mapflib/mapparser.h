#pragma once

#include <istream>
#include <string>
#include <sstream>
#include <ostream>

#include "types.h"

namespace MapFLib {

    enum ParserState {
        PARSER_STATE_ROOT = 0, // What we're at at the start of the file
        PARSER_STATE_ENTITY, // Once we're in an entity
        PARSER_STATE_BRUSH  // When we are in a brush
    };

    int parseMapFile(MapFLib::MapFileData &map, std::istream& stream);

    bool parseEntity(std::istream &stream, MapFLib::MapFileData &map, MapFLib::Entity &entity);
    bool parseEntityProperty(std::istream &stream, std::string &key, std::string &value);
    bool parseEntityBrush(std::istream &stream, MapFLib::MapFileData &map, MapFLib::Brush &brush);
    bool parseBrushFace(std::istream &istream, MapFLib::MapFileData &map, MapFLib::BrushFace &face);
    bool parseVector3(std::istream &stream, MapFLib::Vector3 &vector);
    bool parseTextureAxis(std::istream &stream, MapFLib::TextureAxis &axis);
    bool parseComment(std::istream &stream); // Handles lines that only contain comment that starts with "//"

    void printMapFile(const MapFLib::MapFileData &map, std::ostream &stream);
    void printEntity(const MapFLib::Entity &entity, std::ostream &stream);
    void printEntityBrush(const MapFLib::Brush &b, std::ostream &stream);
    void printBrushFace(const MapFLib::BrushFace &f, std::ostream &stream);

    // Parsing helper functions
    bool expect(std::istream &stream, std::string str);
    bool expectStartsWith(std::istream &stream, std::string str);
    bool expectChar(std::istream &stream, char c);
}
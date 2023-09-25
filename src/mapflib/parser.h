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

    int parseEntityLine(Entity* entity, ParserState &state, std::istream stream);
}
#include "types.h"
#include "parser.h"


using namespace MapFLib;

int MapFLib::parseMapFile(MapFLib::MapFileData &map, std::istream& stream) {
    ParserState state = PARSER_STATE_ROOT;

    Entity *currentEntity = NULL;
    Brush *currentBrush = NULL;

    std::string line;
    while(getline(stream >> std::ws, line)) {

        std::istringstream lineStream(line);

        switch (state) {
        case PARSER_STATE_ROOT:
            if(std::string value; lineStream >> value && value == "{") {
                state = PARSER_STATE_ENTITY;

                currentEntity = new Entity();
            }
            break;
        case PARSER_STATE_ENTITY:
            parseEntityLine(currentEntity, state, lineStream);
            break;
        default:
            break;
        }
    }

    return 0;
}

int MapFLib::parseEntityLine(Entity* entity, ParserState &state, std::istream stream) {
    if((stream >> std::ws).peek() == std::char_traits<char>::to_int_type('"')) {
        
    }
}

std::string read
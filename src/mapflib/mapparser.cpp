#include "types.h"
#include "mapparser.h"

#include <iomanip>
#include <iostream>
#include <sstream>

using namespace MapFLib;

// int MapFLib::parseMapFile(MapFLib::MapFileData &map, std::istream& stream) {
//     ParserState state = PARSER_STATE_ROOT;

//     Entity *currentEntity = NULL;
//     Brush *currentBrush = NULL;

//     std::string line;
//     while(getline(stream >> std::ws, line)) {

//         std::istringstream lineStream(line);

//         switch (state) {
//         case PARSER_STATE_ROOT:
//             if(std::string value; lineStream >> value && value == "{") {
//                 state = PARSER_STATE_ENTITY;

//                 currentEntity = new Entity();
//             }
//             break;
//         case PARSER_STATE_ENTITY:
//             parseEntityLine(currentEntity, state, lineStream);
//             break;
//         default:
//             break;
//         }
//     }

//     return 0;
// }

// int MapFLib::parseEntityLine(Entity* entity, ParserState &state, std::istream stream) {
//     if((stream >> std::ws).peek() == std::char_traits<char>::to_int_type('"')) {
        
//     }
// }

bool expect(std::istream &stream, std::string str) {

    long startLocation = stream.tellg();

    std::string testString;

    stream >> testString;

    if(testString == str) {
        return true;
    } else {
        stream.seekg(startLocation);
        return false;
    }
}

bool MapFLib::parseEntity(std::istream &stream, MapFLib::Entity &entity) {
    long startLocation = stream.tellg();

    if(!expect(stream, "{")) return false; // Syntax Error (entity starts with bracket)

    std::string key, value;
    Brush brush;

    while(true) {
        if(parseComment(stream)) {
            continue;
        } else if(expect(stream, "}")) {
            return true; // Done parsing entity
        } else if(parseEntityProperty(stream, key, value)) {
            entity.properties[key] = value;
        } else if(parseEntityBrush(stream, brush)) {
            entity.brushes.push_back(brush);
            brush = Brush(); // Reset brush
        } else {
            stream.seekg(startLocation);
            return false; // Syntax Error (our only parsing routes failed)
        }
    }
}

bool strictQuoted(std::istream &stream, std::string &s, char delim = '\"') {
    long startLocation = stream.tellg();

    stream >> std::ws;

    if(stream.peek() != (int) delim) {
        // Doesn't start with quote
        stream.seekg(startLocation);
        return false;
    }

    stream >> std::quoted(s);

    if(!stream)
        stream.seekg(startLocation);

    return (bool) stream; // Return its good status
}

bool MapFLib::parseEntityProperty(std::istream &stream, std::string &key, std::string &value) {
    long startLocation = stream.tellg();

    stream >> std::ws;

    if(!strictQuoted(stream, key)) {
        stream.seekg(startLocation);
        return false;
    }

    if(!strictQuoted(stream, value)) {
        stream.seekg(startLocation);
        return false;
    }

    return true;
}

bool MapFLib::parseEntityBrush(std::istream &stream, MapFLib::Brush &brush) {
    return false;
}

bool MapFLib::praseVector3(std::istream &stream, MapFLib::Vector3 &vector);
bool MapFLib::parseTextureAxis(std::istream &stream, MapFLib::Brush brush);

bool MapFLib::parseComment(std::istream &stream) {
    long startLocation = stream.tellg();

    stream >> std::ws;

    if(!expect(stream, "//")) {
        stream.seekg(startLocation);
        return false;
    }

    std::string s;
    std::getline(stream, s); // Just ignore

    return true;
}

int MapFLib::parseMapFile(MapFileData &map, std::istream& stream) {

    Entity entity = Entity();

    while(parseEntity(stream, entity)) {
        // We have an entity
        map.entities.push_back(entity);
        entity = Entity(); // Reinitalize to nothing
    }

    return 0;
}

// std::string read
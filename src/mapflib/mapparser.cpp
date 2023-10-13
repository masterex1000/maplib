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

bool expectStartsWith(std::istream &stream, std::string str) {
    long startLocation = stream.tellg();

    std::string testString;

    stream >> testString;

    if(testString.rfind(str, 0) == 0) {
        return true;
    } else {
        stream.seekg(startLocation);
        return false;
    }
}

bool expectChar(std::istream &stream, char c) {
    long startLocation = stream.tellg();

    stream >> std::ws;

    std::cout << "Value " << stream.peek() << "\n";

    if(stream.peek() != (int) c) {
        stream.seekg(startLocation);
        return false;
    }

    // Succeded, so actually consume char
    stream.get();

    return true;
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
    long startLocation = stream.tellg();

    if(expect(stream, "{")) {
        // printf("Brush: Failed to read first quote\n")
        stream.seekg(startLocation);
        return false;
    }

    BrushFace face;
    while(true) {
        if(expect(stream, "}")) {
            return true;
        } else if(parseBrushFace(stream, face)) {
            brush.faces.push_back(face);
            face = BrushFace(); // Reset brushface
        } else {
            stream.seekg(startLocation);
            return false;
        }
    }

    return false;
}

bool MapFLib::parseBrushFace(std::istream &stream, MapFLib::BrushFace &face) {
    long startLocation = stream.tellg();

    if(!(parseVector3(stream, face.planePoints[0]) &&
         parseVector3(stream, face.planePoints[1]) &&
         parseVector3(stream, face.planePoints[2]))) {
        stream.seekg(startLocation);
        return false;
    }

    stream >> face.imageId;
    if(!stream) {
        stream.seekg(startLocation);
        return false;
    }

    if(!( parseTextureAxis(stream, face.axisU) || parseTextureAxis(stream, face.axisV))) {
        stream.seekg(startLocation);
        std::cout << "Failed to read texture uv data. Make sure map is in valve format (standard format not supported)\n";
        return false;
    }

    stream >> face.rot >> face.scaleX >> face.scaleY;

    if(!stream) {
        stream.seekg(startLocation);
        return false;
    }

    return true;
}

bool MapFLib::parseVector3(std::istream &stream, MapFLib::Vector3 &vector) {
    long startLocation = stream.tellg();

    if(!expectChar(stream, '(')) {
        stream.seekg(startLocation);
        return false;
    }

    stream >> vector.x >> vector.y >> vector.z;

    if(!stream) {
        stream.seekg(startLocation);
        return false;
    }

    if(!expectChar(stream, ')')) {
        stream.seekg(startLocation);
        return false;
    }

    return true;
}

bool MapFLib::parseTextureAxis(std::istream &stream, MapFLib::TextureAxis &axis) {
    long startLocation = stream.tellg();

    if(!expectChar(stream, '[')) {
        stream.seekg(startLocation);
        return false;
    }

    stream >> axis.x >> axis.y >> axis.z >> axis.offset;

    if(!stream) {
        stream.seekg(startLocation);
        return false;
    }

    if(!expectChar(stream, ']')) {
        stream.seekg(startLocation);
        return false;
    }

    return true;
}

bool MapFLib::parseComment(std::istream &stream) {
    long startLocation = stream.tellg();

    stream >> std::ws;

    if(!expectStartsWith(stream, "//")) {
        stream.seekg(startLocation);
        return false;
    }

    std::string s;
    std::getline(stream, s); // Just ignore

    return true;
}

int MapFLib::parseMapFile(MapFileData &map, std::istream& stream) {

    Entity entity = Entity();

    while(true) {
        if(parseComment(stream)) {
            printf("map file read comment\n");
        } else if(parseEntity(stream, entity)) {
            // We have an entity
            map.entities.push_back(entity);
            entity = Entity(); // Reinitalize to nothing
        } else {
            break;
        }
    }

    std::cout << "contains " << map.entities.size() << " entites \n";

    return 0;
}

void const MapFLib::printMapFile(const MapFLib::MapFileData &map) {
    for(const MapFLib::Entity &entity : map.entities) {

        std::cout << "{\n";

        for(auto p : entity.properties) {
            std::cout << "\t" << std::quoted(p.first) << " " << std::quoted(p.second) << "\n";
        }

        std::cout << "\n";

        for(auto b : entity.brushes) {
            std::cout << "\t{\n";

            for(auto f : b.faces) {
                std::cout << "\t\t";

                std::cout << "(" << f.planePoints[0].x << f.planePoints[0].y << f.planePoints[0].z << ") ";
                std::cout << "(" << f.planePoints[1].x << f.planePoints[2].y << f.planePoints[3].z << ") ";
                std::cout << "(" << f.planePoints[2].x << f.planePoints[3].y << f.planePoints[4].z << ") ";

                std::cout << f.imageId;

                std::cout << "[ " << f.axisU.x << f.axisU.y << f.axisU.z << f.axisU.offset << " ] ";
                std::cout << "[ " << f.axisV.x << f.axisV.y << f.axisV.z << f.axisV.offset << " ] ";

                std::cout << f.rot << f.scaleX << f.scaleY << "\n";
            }

            std::cout << "\t}\n";
        }

        std::cout << "}\n\n";
    }
}
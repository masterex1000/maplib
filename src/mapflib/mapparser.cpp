#include "types.h"
#include "mapparser.h"

#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

using namespace MapFLib;

int MapFLib::parseMapFile(MapFileData &map, std::istream& stream) {
    Entity entity = Entity();

    while(true) {
        if(parseComment(stream)) {
            // printf("map file read comment\n");
        } else if(parseEntity(stream, map, entity)) {
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

bool MapFLib::parseEntity(std::istream &stream, MapFLib::MapFileData &map, MapFLib::Entity &entity) {
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
        } else if(parseEntityBrush(stream, map, brush)) {
            entity.brushes.push_back(brush);
            brush = Brush(); // Reset brush
        } else {
            stream.seekg(startLocation);
            return false; // Syntax Error (our only parsing routes failed)
        }
    }
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

bool MapFLib::parseEntityBrush(std::istream &stream, MapFLib::MapFileData &map, MapFLib::Brush &brush) {
    long startLocation = stream.tellg();

    if(!expect(stream, "{")) {
        // printf("Brush: Failed to read first quote\n")
        stream.seekg(startLocation);
        return false;
    }

    BrushFace face;
    while(true) {
        if(expect(stream, "}")) {
            return true;
        } else if(parseBrushFace(stream, map, face)) {
            brush.faces.push_back(face);
            face = BrushFace(); // Reset brushface
        } else {
            stream.seekg(startLocation);
            return false;
        }
    }

    return false;
}

bool MapFLib::parseBrushFace(std::istream &stream, MapFLib::MapFileData &map, MapFLib::BrushFace &face) {
    long startLocation = stream.tellg();

    if(!(parseVector3(stream, face.planePoints[0]) &&
         parseVector3(stream, face.planePoints[1]) &&
         parseVector3(stream, face.planePoints[2]))) {
        stream.seekg(startLocation);
        return false;
    }

    std::string textureName;
    stream >> textureName;

    // Add to texture list if it doesn't already exist
    auto textureQuery = map.textureLookup.find(textureName);
    if(auto search = map.textureLookup.find(textureName); search != map.textureLookup.end()) {
        face.imageId = search->second;
    } else {
        int textureId = map.textureList.size();
        map.textureList.push_back(textureName);
        map.textureLookup[textureName] = textureId;
        face.imageId = textureId;
    }

    if(!stream) {
        stream.seekg(startLocation);
        return false;
    }

    if(!( parseTextureAxis(stream, face.axisU) && parseTextureAxis(stream, face.axisV))) {
        stream.seekg(startLocation);
        std::cout << "Failed to read texture uv data. Make sure map is in valve format (standard texture format not supported)\n";
        // TODO: we totally could support the standard map format
        return false;
    }

    stream >> face.rot >> face.scaleX >> face.scaleY;

    if(!stream) {
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

bool MapFLib::expect(std::istream &stream, std::string str) {
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

bool MapFLib::expectStartsWith(std::istream &stream, std::string str) {
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

bool MapFLib::expectChar(std::istream &stream, char c) {
    long startLocation = stream.tellg();

    stream >> std::ws;

    if(stream.peek() != (int) c) {
        stream.seekg(startLocation);
        return false;
    }

    // Succeded, so actually consume char
    stream.get();

    return true;
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

void MapFLib::printMapFile(const MapFLib::MapFileData &map, std::ostream &stream) {
    for(const MapFLib::Entity &entity : map.entities) {
        printEntity(entity, stream);
    }
}

void MapFLib::printEntity(const MapFLib::Entity &entity, std::ostream &stream) {
    stream << "{\n";

    for(auto p : entity.properties) {
        stream << "\t" << std::quoted(p.first) << " " << std::quoted(p.second) << "\n";
    }

    stream << "\n";

    for(auto b : entity.brushes) {
        printEntityBrush(b, stream);
    }

    stream << "}\n\n";
}

void MapFLib::printEntityBrush(const MapFLib::Brush &b, std::ostream &stream) {
    stream << "\t{\n";

    for(auto f : b.faces) {
        stream << "\t\t";

        printBrushFace(f, stream);
    }

    stream << "\t}\n";
}

void MapFLib::printBrushFace(const MapFLib::BrushFace &f, std::ostream &stream) {
    stream << "(" << f.planePoints[0].x << " " << f.planePoints[0].y << " " << f.planePoints[0].z << ") ";
    stream << "(" << f.planePoints[1].x << " " << f.planePoints[1].y << " " << f.planePoints[1].z << ") ";
    stream << "(" << f.planePoints[2].x << " " << f.planePoints[2].y << " " << f.planePoints[2].z << ") ";

    stream << f.imageId << " ";

    stream << "[ " << f.axisU.x << " " << f.axisU.y << " " << f.axisU.z << " " << f.axisU.offset << " ] ";
    stream << "[ " << f.axisV.x << " " << f.axisV.y << " " << f.axisV.z << " " << f.axisV.offset << " ] ";

    stream << f.rot << " " << f.scaleX << " " << f.scaleY << "\n";
}
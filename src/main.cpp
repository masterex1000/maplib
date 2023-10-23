#include <stdio.h>
#include <fstream>
#include <iostream>
#include <vector>

#include "raymath.h"
#include "stb_ds.h"
#include <mapflib/qma/qmap.h>

#include "types.h"

#include "argh.h"

struct ApplicationOptions {
    bool verbose = false;

    std::string objPath;
    std::string mapFile;

    int textureWidth = 128;
    int textureHeight = 128;
};

using std::cout;
using std::cerr;
using std::endl;


void collectUsedTextures(std::set<int> &usedTextures, QMapEntity *ent) {
    for(int b = 0; b < arrlen(ent->b); b++) {
        QMapBrush *brush = &ent->b[b];

        for(int f = 0; f < arrlen(brush->faces); f++)
            usedTextures.insert(brush->faces[f].texture);
    }
}

void extractBrushGeometry(maptool::ModelBuilder &mb, QMapBrush *brush, QMapBrushGeometry *brush_geo, int texId) {
    for(int f = 0; f < arrlen(brush->faces); f++) {
        QMapFace *face = &brush->faces[f];
        QMapFaceGeometry *face_geo = &brush_geo->faces[f];

        if(face->texture != texId)
            continue;

        for(int i = 0; i < arrlen(face_geo->indices); i++) {
            QMapFaceVertex vert = face_geo->vertices[face_geo->indices[i]];

            Vector3 posVec  = DoubleVector3ToVector3(vert.pos);
            Vector3 normVec = DoubleVector3ToVector3(vert.normal);
            Vector2 texCoords = vert.uv;

            mb.addVertex(posVec, texCoords, normVec);
        }
    }
}

maptool::Model extractMapGeometry(QMapData &map, QMapMapGeometry &map_geo) {
    maptool::ModelBuilder mb;

    for(int e = 0; e < arrlen(map.entitys); e++) {
        QMapEntity *ent = &map.entitys[e];
        QMapEntityGeometry *ent_geo = &map_geo.entitys[e];

        std::ostringstream objectNameStream;
        objectNameStream << "Entity" << e;

        mb.setObject(objectNameStream.str());

        // Populate entity's property list
        for(int p = 0; p < shlen(ent->props); p++) {
            const char *key = ent->props[p].key;
            const char *value = ent->props[p].value;

            mb.addParameter(key, value);
        }

        // Iterate over all of this entity's brushes, building a list of used textures
        std::set<int> usedTextures;

        collectUsedTextures(usedTextures, ent);
        
        for(int texId : usedTextures) {
            std::string textureName = map.textureIndex[texId];

            // Probably should have a way to not hardcode this... but whatever
            if(textureName == "noclip")
                continue;

            mb.setMaterial(textureName);

            for(int b = 0; b < arrlen(ent->b); b++) {
                QMapBrush *brush = &ent->b[b];
                QMapBrushGeometry *brush_geo = &ent_geo->brushes[b];

                extractBrushGeometry(mb, brush, brush_geo, texId);
            }
        }
    }

    return mb.getModel();
}

int runApplication(const ApplicationOptions &options) {

    QMapData map = { 0 };
    QMapMapGeometry map_geo = { 0 };


    // Load map
    if(!map_parse(options.mapFile.c_str(), &map)) {
        cerr << "Unable to load map file '" << options.mapFile << "'" << endl;
        return 1;
    }

    if(options.verbose) {
        cout << "Referenced Textures" << endl << "---------------------------" << endl;
        for(int i = 0; i < arrlen(map.textureIndex); i++) {
            cout << i << " : " << map.textureIndex[i] << endl;
        }
    }

    map_geo = map_generate_geometry(&map);

    maptool::Model mapGeometry = extractMapGeometry(map, map_geo);
    // write to obj file

    // buildObjFile(options, map, map_geo);

    return 0;
}


int main(int argc, char** argv) {
    ApplicationOptions options;

    argh::parser cmdl;
    cmdl.add_params({
        "-w", "--textureWidth",
        "-h", "--textureHeight",
        "-o", "--output"
    });

    cmdl.parse(argc, argv);

    if(cmdl[{"-h", "--help"}]) {
        cout << "Usage: " << cmdl[0] << " [OPTION]... mapfile\n";
        cout << "      --version    display the version of the program" << endl;
        cout << "  -v, --verbose    output a diagnostic thought map compile process\n";
        cout << "  -w, --textureWidth=...   Sets the texture size used by the compiler to calculate UVs" << endl;
        cout << "  -h, --textureHeight=...  Sets the texture size used by the compiler to calculate UVs" << endl;
        cout << "  -o, --output=... Sets the path that the output obj file will be located at (defaults to a.obj)" << endl;
        cout << "      --help   display this help and exit" << endl;

        return 0;
    }

    options.verbose = cmdl[{"-v", "--help"}];
    
    if(!(cmdl({"-w", "--textureWidth"}, options.textureWidth) >> options.textureWidth)) {
        cerr << "Invalid width parameter '" << cmdl({"-w", "textureWidth"}).str() << "'" << endl;
        return 1;
    }

    if(!(cmdl({"-h", "--textureHeight"}, options.textureHeight) >> options.textureHeight)) {
        cerr << "Invalid height parameter '" << cmdl({"-h", "textureHeight"}).str() << "'" << endl;
        return 1;
    }

    options.objPath = cmdl({"-o", "--output"}, "a.obj").str();

    if(!cmdl(1)) {
        cerr << "No map file provided" << endl;
        return 1;
    }

    options.mapFile = cmdl(1).str();

    // cout << "Positional args:\n";
    // for (auto& pos_arg : cmdl)
    //     cout << '\t' << pos_arg << '\n';

    return runApplication(options);
}
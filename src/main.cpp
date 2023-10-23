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

maptool::Model extractMapGeometry(QMapData &map, QMapMapGeometry &map_geo) {
    maptool::ModelBuilder mb;

    int asdf = arrlen(map.entitys);

    for(int e = 0; e < arrlen(map.entitys); e++) {
        QMapEntity *ent = &map.entitys[e];
        QMapEntityGeometry *ent_geo = &map_geo.entitys[e];

        // Iterate over all of this entity's brushes, building a list of used textures
        std::set<int> usedTextures;

        std::ostringstream objectNameStream;
        objectNameStream << "Entity" << e;

        mb.setObject(objectNameStream.str());

        for(int b = 0; b < arrlen(ent->b); b++) {
            QMapBrush *brush = &ent->b[b];

            for(int f = 0; f < arrlen(brush->faces); f++)
                usedTextures.insert(brush->faces[f].texture);
        }

        for(int texId : usedTextures) {
            mb.setMaterial(map.textureIndex[texId]);

            for(int b = 0; b < arrlen(ent->b); b++) {
                QMapBrush *brush = &ent->b[b];
                QMapBrushGeometry *brush_geo = &ent_geo->brushes[b];

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
        }
    }

    return mb.getModel();
}


// void (const ApplicationOptions &options, const QMapData &map, const QMapMapGeometry &map_geo) {
    // Extract all entity geometry in map
    // Write obj file
// }

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

    // extract map geometry
    maptool::Model mapGeometry = extractMapGeometry(map, map_geo);
    // write to obj file
    // write entity definitions (probably just in the obj file itself as a shitty unoffical extension)

    // buildObjFile(options, map, map_geo);

    return 0;
}


int main(int argc, char** argv) {
    ApplicationOptions options;

    argh::parser cmdl;
    cmdl.add_params({
        "-w", "--textureWidth",
        "-h", "--textureHeight",
        "-o", "--objpath"
    });

    cmdl.parse(argc, argv);

    if(cmdl[{"-h", "--help"}]) {
        cout << "Usage: " << cmdl[0] << " [OPTION]... mapfile\n";
        cout << "      --version    display the version of the program" << endl;
        cout << "  -v, --verbose    output a diagnostic thought map compile process\n";
        cout << "  -w, --textureWidth=...   Sets the texture size used by the compiler to calculate UVs" << endl;
        cout << "  -h, --textureHeight=...  Sets the texture size used by the compiler to calculate UVs" << endl;
        cout << "  -o, --objpath=...    Sets the path to store the obj files used to repersent entity brush data" << endl;
        cout << "                       defaults to cwd" << endl;
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

    options.objPath = cmdl({"-o", "--objpath"}, "./").str();

    if(!cmdl(1)) {
        cout << "No map file provided" << endl;
        return 1;
    }

    options.mapFile = cmdl(1).str();

    // cout << "Positional args:\n";
    // for (auto& pos_arg : cmdl)
    //     cout << '\t' << pos_arg << '\n';

    return runApplication(options);
}
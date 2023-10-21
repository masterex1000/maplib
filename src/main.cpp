#include <stdio.h>
#include <fstream>
#include <iostream>

#include "raymath.h"
#include <mapflib/qma/qmap.h>

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


int runApplication(const ApplicationOptions &options) {
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

    // TODO: add message if no filename

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

    // cout << "Positional args:\n";
    // for (auto& pos_arg : cmdl)
    //     cout << '\t' << pos_arg << '\n';

    return runApplication(options);
}
#include <stdio.h>
#include <fstream>

#include "mapflib/mapparser.h"

int testStuff() {
    MapFLib::MapFileData map;

    std::fstream fs;
    fs.open("../test/basic.map", std::fstream::in);

    MapFLib::parseMapFile(map, fs);

    return 0;
}

int main(int argc, char** argv) {
    return testStuff();
}
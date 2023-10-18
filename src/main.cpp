#include <stdio.h>
#include <fstream>
#include <iostream>

#include "mapflib/mapparser.h"

int testStuff() {
    MapFLib::MapFileData map;
    // MapFLib::MapGeometry mapGeo;

    std::fstream fs;
    fs.open("../test/minimal.map", std::fstream::in | std::ios::binary);

    MapFLib::parseMapFile(map, fs);

    MapFLib::printMapFile(map, std::cout); // Test if we have a similar output

    // MapFLib::generateMeshData(map);

    return 0;
}

int main(int argc, char** argv) {
    return testStuff();
}
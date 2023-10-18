#include <stdio.h>
#include <fstream>
#include <iostream>

#include "mapflib/mapparser.h"

int testStuff() {
    MapFLib::MapFileData map;

    std::fstream fs;
    fs.open("../test/minimal.map", std::fstream::in | std::ios::binary);

    MapFLib::parseMapFile(map, fs);

    MapFLib::printMapFile(map, std::cout); // Test if we have a similar output

    return 0;
}

int main(int argc, char** argv) {
    return testStuff();
}
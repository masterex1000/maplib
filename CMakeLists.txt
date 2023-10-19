cmake_minimum_required(VERSION 3.12)

project(maptool)

# list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# find_package(Eigen3 REQUIRED)

# message(${EIGEN3_INCLUDE_DIR})
# message(${CMAKE_MODULE_PATH})

add_executable( maptool
    src/main.cpp

    src/mapflib/mapparser.cpp
)

target_compile_features(maptool PUBLIC cxx_std_17)
target_include_directories(maptool PUBLIC include/ src/)
target_compile_options(maptool PRIVATE -Wall)

add_executable( testmapflib
    src/test.cpp

    src/mapflib/mapparser.cpp
    src/mapflib/mapbuilder.cpp
)

target_compile_features(testmapflib PUBLIC cxx_std_17)
target_include_directories(testmapflib PUBLIC include/ src/)
target_compile_options(testmapflib PRIVATE -Wall)
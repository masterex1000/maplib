cmake_minimum_required(VERSION 3.12)

project(maptool)

add_executable( maptool
    src/main.cpp
    src/ModelBuilder.cpp

    src/mapflib/qma/qmap.cpp
)

target_compile_features(maptool PUBLIC cxx_std_17)
target_include_directories(maptool PUBLIC include/ src/)
target_compile_options(maptool PRIVATE -Wall)

# add_executable( testmapflib
#     src/test.cpp

#     src/mapflib/qma/qmap.cpp
# )

# target_compile_features(testmapflib PUBLIC cxx_std_17)
# target_include_directories(testmapflib PUBLIC include/ src/)
# target_compile_options(testmapflib PRIVATE -Wall)

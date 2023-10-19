# .MAP tool

A small and simple generic `.map` file parser/converter/generator.

There are two major pieces to this project
 - mapflib - .map parser/generator in library form
 - maptool - cli frontend to the library that can output .obj and entity definition (in a custom, very simple to parse format) files


## Dependencies

To install, download Eigen3. Build with cmake (outlined in INSTALL file in repo)
Set `EIGEN3_ROOT` enviroment variable to root of install folder
Done!

# Helpful Links

Just a couple links that are helpful for understanding the project/format

https://book.leveldesignbook.com/appendix/resources/formats/map


# Grammer

--- Defined by parser ---
Number
String
Quoted

--- Parse Rules ---

Root -> Entity*

Entity -> "{" (Property | Brush )* "}"
Property -> Quoted Quoted
Brush -> "{" Vector3 Vector3 Vector3 String TextureAxis TextureAxis Number Number Number "}"

Vector3 -> "(" Number Number Number ")"
TextureAxis -> "[" Number Number Number "]"
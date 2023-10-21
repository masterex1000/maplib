# .MAP tool

A small and simple generic `.map` file parser/converter/generator.

There are two major pieces to this project
 - mapflib - .map parser/generator in library form
 - maptool - cli frontend to the library that can output .obj and entity definition files (in a custom, very simple to parse format) files


## Maptool

### Usage

```
maptool --texwidth 128 --texheight 128 -t "./Temp/" mymap.map
```
This will compile the given map, using a "fake texture" width/height of 128 (must match texture sizes used in editor)
using `./Temp/` as the temporary directory to put entity brush obj files into.

### Output format

The output format of the program is very similar to the .map files this program reads, except instead of complicated brush data
we instead have a single .obj file associated with every entity (that has brush data associated with it)

NOTE: the obj files *DO NOT* have an associated `.mtl` file with them. Instead, because this project is primarily designed to be
used with unity, its assumed that the importer (which because you can't manually programmatically load obj files in unity) is custom
and will be able to resolve the material names itself to ones that exist in the project already.

```
{ "classname" "worldspawn" { "entity0.obj" } }
{ "classname" "func_group" { "entity1.obj" } }
```

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
# .MAP tool

A small and simple generic `.map` file parser/converter/generator that spits out slightly custom `.obj` files.

## Maptool

### Output format

The output of this program is a effectively just a very slightly modified `.obj` file. It is the exact same as a standard obj file, except that
it has a custom `eparam` extension to represent an entity's parameters.

eg.

```obj
...
o Entity0
useMtl grass_texture
f 1/2/3 1/2/3 1/2/3
useMtl rock_texture
f 1/2/3 1/2/3 1/2/3
f ...
...
eparam "classname" "worldspawn"
```

From the above you should see the only real difference between a normal obj and this program's is the `eparam` keyword, which is used
to represent a single entity parameter.

```
eparam "<key>" "<value>"
```

The extension simply consists of the `eparam` keyword, followed by a key/value pair. It should just be ignored by most readers,
meaning this extension only does something on a compatible parser (eg. my custom one written for unity)

NOTE: the obj files *DO NOT* have an associated `.mtl` file with them. Instead, because this project was built with a custom .obj Unity importer in mind its left up to the importer to resolve the material names itself.

# TODO

Theres still a lot of stuff I want to do with this project, but for now its in a semi-functional state.

- Make parser less fragile
    - Currently the parser *heavily* relies on reading a specific style of map file. Eg.
    comments *need* to take up an entire line with nothing else, parenthesis have to be in the
    right places... It gets really annoying really fast. The goal would be to generalize and make it more robust.
- Rewrite parser
    - Yeah, right below generalizing it. I've got another implementation on another branch, but
    it doesn't have any of the geometry generating code, so that'll be a TODO right there
- Proper memory managment
    - Currently qma (the subfolder for all the map code) is pretty lose with
    its memory managment. Although its generally good at cleaning up after its own functions, theres no real
    way to free memory from the objects it returns, so its just leaks all over the place.
- Handle textures of different sizes
    - Hopefully a rewrite would be able to address this too. Currently
    textures are fixed at a single size, and thats the size used for everything.
- Allow outputing special types of meshes
    - Eg. collision meshes. This would idealy also take into account
    things like tool brushes
- CSG
    - A HUGE one, but pretty nice to have. Theres a lot of hidden faces that could be culled out of the
    map without any drawbacks. Its mostly because I haven't been able to wrap my head around any CSG algorithms yet.

Most of these are specific to my qma library, which is a project I originally built a few years ago. I've have just been
dragging it along from project to project while willingly ignoring all of its issues, so its got a bit a jank in it.

My ultimate goal with this project is to eventually build a *decent* map file parser/generator library which just happens
to ship with a nice cli too.

# Helpful Links

Just a couple links that are helpful for understanding the project/format

https://book.leveldesignbook.com/appendix/resources/formats/map


# `.map` file grammer

```
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
```
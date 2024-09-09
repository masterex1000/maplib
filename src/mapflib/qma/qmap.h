#include <stdbool.h>
#include <math.h>

#ifndef QMAP_H
#define QMAP_H

#define CMP_EPSILON 0.00001

//
// Geometry
//

typedef struct {
    double x;
    double y;
    double z;
} DoubleVector3;

typedef struct {
    DoubleVector3 pos;
    DoubleVector3 normal;
    Vector2 uv;
    Vector4 tangent;
} QMapFaceVertex;

typedef struct {
    QMapFaceVertex *vertices;
    int *indices;
} QMapFaceGeometry;

typedef struct {
    QMapFaceGeometry *faces;
} QMapBrushGeometry;

typedef struct {
    QMapBrushGeometry *brushes;
} QMapEntityGeometry;

typedef struct {
    QMapEntityGeometry *entitys;
} QMapMapGeometry;

//
// Map Datastruct
//

typedef struct {
    DoubleVector3 axis;
    double offset;
} QMapValveTextureAtlas;

typedef struct {
    DoubleVector3 v0;
    DoubleVector3 v1;
    DoubleVector3 v2;

    DoubleVector3 normal;
    double dist;

    int texture;

    bool isValve;

    // Common Texture Values
    double tRot;
    double tScaleX;
    double tScaleY;

    // Type Specific Texture Values
    union {
        struct { // Valve Textures 
            QMapValveTextureAtlas vU;
            QMapValveTextureAtlas vV;
        };

        struct { // Standard Textures
            double sU;
            double sV;
        };
    };
    
} QMapFace;

typedef struct {
    QMapFace *faces;
    DoubleVector3 center;
} QMapBrush;

typedef struct {
    QMapBrush *b;
    struct {char *key; char *value; } *props; // stb_ds string hash table
    DoubleVector3 center;
} QMapEntity;

typedef struct {
    QMapEntity *entitys;        // stb_ds array
    char **textureIndex;    // stb_ds array
    Vector2 *textureSizes;  // stb_ds array
} QMapData;

typedef struct {
    int parser_state;
    QMapData map;

    QMapEntity curEntity;
    QMapBrush curBrush;
} QMapParserState;

//
// Entity Manipulation Functions
//

inline size_t qmap_entity_getId(QMapData* map, QMapEntity* entity) {
    return (size_t) (entity - map->entitys);
}

const char* qmap_entity_getProperty(QMapEntity* entity, const char* propName); // Attempts to get a property from an entity, returning null if it doesn't exist
const char* qmap_entity_getProperty(QMapData* map, size_t entityId, const char* propName); // Attempts to get a property from an entity, returning null if it doesn't exist

//
// Math
//

inline Vector3 DoubleVector3ToVector3(DoubleVector3 d) {
    return (Vector3) {(float) d.x, (float) d.y, (float) d.z};
}

typedef struct mat4
{
    double m[16];
} mat4;

#ifndef PI
    #define PI 3.14159265358979323846f
#endif

#define DEG_TO_RAD(deg) (deg * (PI / 180))
#define RAD_TO_DEG(deg) (deg * (180 / PI))

// Vector Math

bool vec3_equals(DoubleVector3 lhs, DoubleVector3 rhs);
DoubleVector3 vec3_add(DoubleVector3 lhs, DoubleVector3 rhs);
DoubleVector3 vec3_sub(DoubleVector3 lhs, DoubleVector3 rhs);
DoubleVector3 vec3_mul(DoubleVector3 lhs, DoubleVector3 rhs);
DoubleVector3 vec3_div(DoubleVector3 lhs, DoubleVector3 rhs);

DoubleVector3 vec3_add_double(const DoubleVector3 lhs, const double rhs);
DoubleVector3 vec3_sub_double(const DoubleVector3 lhs, const double rhs);
DoubleVector3 vec3_mul_double(const DoubleVector3 lhs, const double rhs);
DoubleVector3 vec3_div_double(const DoubleVector3 lhs, const double rhs);

double vec3_dot(DoubleVector3 lhs, DoubleVector3 rhs);
DoubleVector3 vec3_cross(DoubleVector3 lhs, DoubleVector3 rhs);
double vec3_sqlen(DoubleVector3 v);
double vec3_length(DoubleVector3 v);
DoubleVector3 vec3_normalize(DoubleVector3 v);
DoubleVector3 vec3_rotate(DoubleVector3 v, DoubleVector3 axis, double angle);

// Matrix
mat4 mat4_identity();
//vec4 mat4_mul_vec4(mat4 m, vec4 v);
DoubleVector3 mat4_mul_vec3(mat4 m, DoubleVector3 v);
double get_fovy(double fov_x, double aspect);
mat4 rotation_matrix(DoubleVector3 axis, double angle);
mat4 projection_matrix(double fovy_degrees, double aspect, double z_near, double z_far, bool flip_fov);

#endif

// Include Sub Headers
#include "vector.h"
#include "matrix.h"
#include "parser.h"
#include "geometry.h"
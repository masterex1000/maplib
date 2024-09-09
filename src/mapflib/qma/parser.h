#ifndef QMAP_PARSER_H
#define QMAP_PARSER_H

bool map_parse(const char *map_file, QMapData *data);
bool map_parse_line(QMapParserState *state, char *buf, size_t buf_len);
bool map_parse_param(const char *str, char **name, char **value);
bool map_parse_brush_face(QMapParserState *state, const char *str, QMapFace *face);

QMapData map_mapdata_init();
QMapEntity map_entity_init();
QMapBrush map_brush_init();

#endif

#ifdef QMAP_IMP

extern int map_defaultTextureWidth;
extern int map_defaultTextureHeight;

#define MPP_FILE 0
#define MPP_ENTITY 1
#define MPP_BRUSH 2

bool map_parse(const char *map_file, QMapData *data) {
    FILE *file = fopen(map_file, "r");

    if(!file) {
        printf("ERROR: Couldn't open map file\n");
        return false;
    }

    int c;
    // char buf[255];
    const size_t buf_size = 1024;
    char buf[buf_size];
    int buf_len = 0;

    QMapParserState pstate;
    pstate.parser_state = MPP_FILE;

    pstate.map = map_mapdata_init();

    memset(buf, 0, buf_size);

    while((c = fgetc(file)) != EOF) {
        if(c == '\n') {
            // printf("got line : %s\n", buf);

            if(!map_parse_line(&pstate, buf, buf_len)) {
                printf("ERROR: Parse error\n");
                return false;
            }
            
            memset(buf, 0, buf_size);
            buf_len = 0;
        } else {
            buf[buf_len] = c;
            buf_len++;

            if(buf_len > (int) buf_size) {
                printf("ERROR: Token To long\n");
                return false;
            }
        }
    }

    *data = pstate.map;
    return true;
}

bool map_parse_line(QMapParserState *state, char *buf, size_t buf_len) {
    // printf("Parsing Line : %s\n", buf);

    if(strncmp(buf, "//", 2) == 0) { // We have a comment, skip
        return true;
    } else if(state->parser_state == MPP_FILE) {
        if(strncmp(buf, "{", 1) == 0) {
            state->parser_state = MPP_ENTITY;
            // printf("[ -- New QMapEntity -- ]\n");

            state->curEntity = map_entity_init();
        } else
            return false; // Unknown line
    } else if(state->parser_state == MPP_ENTITY) {
        if(strncmp(buf, "{", 1) == 0) {
            state->parser_state = MPP_BRUSH;
            state->curBrush = map_brush_init();
            
            // printf("\t[ -- New QMapBrush -- ]\n");
        } else if(strncmp(buf, "}", 1) == 0) {
            state->parser_state = MPP_FILE;
            // printf("[ -- End of entity -- ]\n");

            arrput(state->map.entitys, state->curEntity);
        } else {
            char *pname;
            char *pval;

            if(!map_parse_param(buf, &pname, &pval)) {
                return false; // Not a valid line
            }

            // printf("\t[ Name : '%s', Value : '%s' ]\n", pname, pval);

            shput(state->curEntity.props, pname, pval);
        }
    } else if(state->parser_state == MPP_BRUSH) {
        if(strncmp(buf, "}", 1) == 0) {
            state->parser_state = MPP_ENTITY;
            arrput(state->curEntity.b, state->curBrush);
            // printf("\t[ -- End of QMapBrush -- ]\n");
        } else {
            QMapFace fce;

            if(!map_parse_brush_face(state, buf, &fce))
                return false;

            arrput(state->curBrush.faces, fce);
        }
    }
    return true;
}

bool map_parse_param(const char *str, char **name, char **value) {
    char n[255];
    char v[255];

    memset(n, 0, 255);
    memset(v, 0, 255);

    int count = sscanf(str, " \"%254[^\"]\" \"%254[^\"]\"", n, v);

    if(count < 2) // We need both fields, otherwise the line is bad
        return false;

    char *nameval = (char*) malloc(sizeof(char) * strlen(n) + 1);
    memset(nameval, 0, sizeof(char) * strlen(n) + 1);
    memcpy(nameval, n, strlen(n));

    char *valval = (char*) malloc(sizeof(char) * strlen(v) + 1);
    memset(valval, 0, sizeof(char) * strlen(v) + 1);
    memcpy(valval, v, strlen(v));

    *name = nameval;
    *value = valval;

    return true;
}

bool map_parse_brush_face(QMapParserState *state, const char *str, QMapFace *face) {

    // Note (for reference)
    // Quake Style Brush:
    // ( x1 y1 z1 ) ( x2 y2 z2 ) ( x3 y3 z3 ) TEXTURE Xoffset Yoffset rotation Xscale Yscale
    // Valve Style Brush:
    // ( x1 y1 z1 ) ( x2 y2 z2 ) ( x3 y3 z3 ) TEXTURE [ Tx1 Ty1 Tz1 Toffset1 ] [ Tx2 Ty2 Tz2 Toffset2 ] rotation Xscale Yscale

    char texture_name[255];
    int position = 0;

    memset(texture_name, 0, 255);

    int count = sscanf(str, " ( %lf %lf %lf ) ( %lf %lf %lf ) ( %lf %lf %lf ) %254[a-zA-Z0-9_/] %n",
        &face->v0.x, &face->v0.y, &face->v0.z,
        &face->v1.x, &face->v1.y, &face->v1.z,
        &face->v2.x, &face->v2.y, &face->v2.z,
        texture_name, &position);

    // We need to calculate the planes normal and distance
    DoubleVector3 v0v1 = vec3_sub(face->v1, face->v0);
    DoubleVector3 v1v2 = vec3_sub(face->v2, face->v1);
    face->normal = vec3_normalize(vec3_cross(v1v2, v0v1));
    face->dist = vec3_dot(face->normal, face->v0);

    bool isValid = false;

    count = sscanf(str + position, " [ %lf %lf %lf %lf ] [ %lf %lf %lf %lf ] %lf %lf %lf",
        &face->vU.axis.x, &face->vU.axis.y, &face->vU.axis.z, &face->vU.offset,
        &face->vV.axis.x, &face->vV.axis.y, &face->vV.axis.z, &face->vV.offset,
        &face->tRot, &face->tScaleX, &face->tScaleY);

    // We parsed the string! it is an valve format brush!
    if(count == 11) {
        face->isValve = true;
        isValid = true;
    }

    // We didn't parse the string... looks like we have a quake format brush

    if(!isValid) { 
        count = sscanf(str + position, " %lf %lf %lf %lf %lf",
            &face->sU, &face->sV, &face->tRot, &face->tScaleX, &face->tScaleY);

        // We better make sure we actually have a quake format brush, just to make
        // sure there isn't any funny stuff going on
        if(count == 5) {
            face->isValve = false;
            isValid = true;
        }
    }

    // If After all this and don't even have a valid brush we better tell the parser
    if(!isValid) {
        printf("The Dang brush failed\n");
        return false;
    }

    // We also need to register the texture in the mapdata

    for(int i = 0; i < arrlen(state->map.textureIndex); i++) {
        if(strcmp(state->map.textureIndex[i], texture_name) == 0) {
            face->texture = i;
            return true;
        }
    }

    // We didn't find the texture already, we need to add a new listing
    char *texName = (char*) malloc(strlen(texture_name) + 1);
    memcpy(texName, texture_name, strlen(texture_name) + 1);
    
    face->texture = arrlen(state->map.textureIndex);
    arrput(state->map.textureIndex, texName);
    // arrput(state->map.textureSizes, ((Vector2) {128, 128})); // Just set a default size
    arrput(state->map.textureSizes, ((Vector2) {(float) map_defaultTextureWidth, (float) map_defaultTextureHeight})); // Just set a default size

    return true;
}


QMapData map_mapdata_init() {
    QMapData m;
    m.entitys = NULL;
    m.textureIndex = NULL;
    m.textureSizes = NULL;
    return m;
}

QMapEntity map_entity_init() {
    QMapEntity e;
    e.b = NULL;
    e.center = (DoubleVector3) {0, 0, 0};
    e.props = NULL;
    return e;
}

QMapBrush map_brush_init() {
    QMapBrush b;
    b.faces = NULL;
    b.center = (DoubleVector3) {0, 0, 0};
    return b;
}

const char* qmap_entity_getProperty(QMapEntity* entity, const char* propName) {
    return shget(entity->props, propName);
}

const char* qmap_entity_getProperty(QMapData* map, size_t entityId, const char* propName) {
    return shget(map->entitys[entityId].props, propName);
}
#endif
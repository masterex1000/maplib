#ifndef MAP_SURFACE_QUERY
#define MAP_SURFACE_QUERY

#define SURFACE_BOTH               0   // Include everything
#define SURFACE_ONLY_WRLDSPWN    1   // Only worldspawn (and func_group)
#define SURFACE_NO_WRLDSPWN     2   // No Worldspawn (and func_group)

typedef struct {
    QMapData *map;
    QMapMapGeometry *map_geo;

    bool filterTexture;     // Selects if we should filter by texture
    int filterTextureID;    // The texture to filter by if we are texture filtering

    bool filterEntity;      // Selects if we will filter by entity;
    int filterEntityID;

    int worldspawnMode;     // Uses SURFACE_MODE_* defines to configure
} SurfaceQuery;


SurfaceQuery surfacequery_new(QMapData *map, QMapMapGeometry *geo);

// Collects the surfaces that match the query, returns the number of faces
Mesh surfacequery_collect(SurfaceQuery *query, int *count, bool uploadMesh = true);

#endif

#ifdef QMAP_IMP

bool surfacequery_isworldspawn(QMapEntity *ent) {
    if(shgeti(ent->props, "classname") == -1)
        return true;
    
    char *classname = shget(ent->props, "classname");

    if(strncmp("worldspawn", classname, 10) || strncmp("func_group", classname, 10))
        return true;

    return false;
}


Mesh surfacequery_collect(SurfaceQuery *query, int *count, bool uploadMesh) {
    Mesh m = { 0 };

    if(!query->map)
        return m; // Just fail

    if(uploadMesh)
        m.vboId = (unsigned int *) calloc(7, sizeof(unsigned int));
    // m.vertexCount = 0;
    // m.triangleCount = 0;
    
    int collectedFaces = 0;
    int indexNum = 0;
    Vector3 *verts = NULL; // an stb_ds array, will be copied into the mesh and freed
    Vector3 *normals = NULL; // STBDS array
    Vector2 *texCoords = NULL;
    int *triangles = NULL;

    for(int e = 0; e < arrlen(query->map->entitys); e++) {
        QMapEntity *ent = &query->map->entitys[e];
        QMapEntityGeometry *ent_geo = &query->map_geo->entitys[e];

        if(query->worldspawnMode == SURFACE_NO_WRLDSPWN && surfacequery_isworldspawn(ent))
            continue; // This entity was a part of worldspawn

        if(query->worldspawnMode == SURFACE_ONLY_WRLDSPWN && !(surfacequery_isworldspawn(ent)))
            continue; // This entity wasn't a part of world spawn

        if(query->filterEntity && e != query->filterEntityID)
            continue; // This entity didn't have the right entity ID

        for(int b = 0; b < arrlen(ent->b); b++) {
            QMapBrush *brush = &ent->b[b];
            QMapBrushGeometry *brush_geo = &ent_geo->brushes[b];

            for(int f = 0; f < arrlen(brush->faces); f++) {
                QMapFace *face = &brush->faces[f];
                QMapFaceGeometry *face_geo = &brush_geo->faces[f];

                if(query->filterTexture && face->texture != query->filterTextureID)
                    continue; // This face was filtered out

                collectedFaces++;

                // printf("QMapFace: ");
                for(int v = 0; v < arrlen(face_geo->vertices); v++) {
                    QMapFaceVertex vert = face_geo->vertices[v];

                    // printf("{%f, %f, %f} ", vert.pos.x, vert.pos.y, vert.pos.z);
                    arrput(verts, DoubleVector3ToVector3(vert.pos));
                    arrput(normals, DoubleVector3ToVector3(vert.normal));
                    arrput(texCoords, vert.uv);
                    //TODO Do we need the tangent?
                }
                
                // printf("[ ");
                for(int i = 0; i < arrlen(face_geo->indices); i++) {
                    arrput(triangles, face_geo->indices[i] + indexNum);
                    // printf("%d ", face_geo->indices[i] + indexNum);
                }
                // printf("]\n");

                indexNum += arrlen(face_geo->vertices);
            }
        }
    }

    // Move all of the data to the mesh

    m.vertexCount = arrlen(verts);
    m.triangleCount = arrlen(triangles) / 3;
    m.vertices = (float *)malloc(m.vertexCount*3*sizeof(float));
    m.texcoords = (float *)malloc(m.vertexCount*2*sizeof(float));
    m.normals = (float *)malloc(m.vertexCount*3*sizeof(float));
    m.indices = (unsigned short *)malloc(m.triangleCount*3*sizeof(unsigned short));

    // Mesh vertices position array
    for (int i = 0; i < m.vertexCount; i++)
    {
        // printf("{%f, %f, %f}\n", verts[i].x, verts[i].y, verts[i].z);
        m.vertices[3*i] = verts[i].y;
        m.vertices[3*i + 1] = verts[i].z;
        m.vertices[3*i + 2] = verts[i].x;
    }

    // Mesh texcoords array
    for (int i = 0; i < m.vertexCount; i++)
    {
        m.texcoords[2*i] = texCoords[i].x;
        m.texcoords[2*i + 1] = texCoords[i].y;
    }

    // Mesh normals array
    for (int i = 0; i < m.vertexCount; i++) {
        m.normals[3*i] = normals[i].x;
        m.normals[3*i + 1] = normals[i].y;
        m.normals[3*i + 2] = normals[i].z;
    }

    // Mesh indices array initialization
    for (int i = 0; i < m.triangleCount*3; i++) m.indices[i] = triangles[i];

    arrfree(verts);
    arrfree(normals);
    arrfree(texCoords);
    arrfree(triangles);

    *count = collectedFaces;

    if(uploadMesh)
        UploadMesh(&m, false);
    

    return m;
}

SurfaceQuery surfacequery_new(QMapData *map, QMapMapGeometry *geo) {
    SurfaceQuery query;

    query.map = map;
    query.map_geo = geo;

    return query;
}

#endif
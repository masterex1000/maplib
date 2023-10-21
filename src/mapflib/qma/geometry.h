#ifndef GEOMETRY_H
#define GEOMETRY_H

QMapMapGeometry map_generate_geometry(QMapData *map);

QMapEntityGeometry map_entity_geo_init();
QMapBrushGeometry map_brush_geo_init();
QMapFaceGeometry map_face_geo_init();

void map_generate_brush_geometry(QMapBrush *b, QMapBrushGeometry *g, bool phong, QMapData *m);
void map_print_geometry(const QMapData *map, const QMapMapGeometry *geo);

bool intersect_faces(QMapFace f0, QMapFace f1, QMapFace f2, DoubleVector3 *o_vertex);
bool vertex_in_hull(QMapFace *faces, int face_count, DoubleVector3 vertex);

Vector2 map_get_uv(DoubleVector3 vertex, const QMapFace *f, Vector2 texSize);
Vector4 map_get_tangent(QMapFace *face);

#endif

#ifdef QMAP_IMP

QMapData *wind_map_data = NULL;
QMapMapGeometry *wind_map_geo = NULL;
int wind_entity_idx = 0;
int wind_brush_idx = 0;
int wind_face_idx = 0;
DoubleVector3 wind_face_center;
DoubleVector3 wind_face_basis;
DoubleVector3 wind_face_normal;

int sort_vertices_by_winding(const void *lhs_in, const void *rhs_in) {
    const DoubleVector3 *lhs = (const DoubleVector3 *)lhs_in;
    const DoubleVector3 *rhs = (const DoubleVector3 *)rhs_in;

    // QMapFace *face_inst = &wind_map_data->entitys[wind_entity_idx].b[wind_brush_idx].faces[wind_face_idx];
    // QMapFaceGeometry *face_geo_inst = &wind_map_geo->entitys[wind_entity_idx].brushes[wind_brush_idx].faces[wind_face_idx];

    DoubleVector3 u = vec3_normalize(wind_face_basis);
    DoubleVector3 v = vec3_normalize(vec3_cross(u, wind_face_normal));

    DoubleVector3 local_lhs = vec3_sub(*lhs, wind_face_center);
    double lhs_pu = vec3_dot(local_lhs, u);
    double lhs_pv = vec3_dot(local_lhs, v);

    DoubleVector3 local_rhs = vec3_sub(*rhs, wind_face_center);
    double rhs_pu = vec3_dot(local_rhs, u);
    double rhs_pv = vec3_dot(local_rhs, v);

    double lhs_angle = atan2(lhs_pv, lhs_pu);
    double rhs_angle = atan2(rhs_pv, rhs_pu);

    if (lhs_angle < rhs_angle) {
        return 1;
    }
    else if (lhs_angle > rhs_angle) {
        return -1;
    }

    return 0;
}

QMapMapGeometry map_generate_geometry(QMapData *map) {
    QMapMapGeometry geo;
    geo.entitys = NULL;

    // Initalize all of our objects, make sure all the arrays are allocated etc...
    for(int i = 0; i < arrlen(map->entitys); i++) {
        arrput(geo.entitys, map_entity_geo_init());

        QMapEntity *ent = &(map->entitys[i]);
        QMapEntityGeometry *ent_geo = &(geo.entitys[i]);

        for(int j = 0; j < arrlen(ent->b); j++) {
            arrput(ent_geo->brushes, map_brush_geo_init());

            QMapBrush *brush = &(ent->b[j]);
            QMapBrushGeometry *brush_geo = &(ent_geo->brushes[j]);

            for(int k = 0; k < arrlen(brush->faces); k++) {
                arrput(brush_geo->faces, map_face_geo_init());
            }
        }
    }

    // Generate brush vertices and calculate their centers
    for(int e = 0; e < arrlen(map->entitys); e++) {
        QMapEntity *ent = &(map->entitys[e]);
        QMapEntityGeometry *ent_geo = &(geo.entitys[e]);

        ent->center = (DoubleVector3) {0, 0, 0};

        for(int b = 0; b < arrlen(ent->b); b++) {
            QMapBrush *brush = &(ent->b[b]);
            QMapBrushGeometry *brush_geo = &(ent_geo->brushes[b]);
            
            int vertex_count = 0;
            brush->center = (DoubleVector3) {0, 0, 0};

            // TODO implement phong shading parameter test! (Maybe, im just going to default to phong shading for now....)
            map_generate_brush_geometry(brush, brush_geo, true, map);

            for(int f = 0; f < arrlen(brush_geo->faces); f++) {
                QMapFaceGeometry *face_geo = &brush_geo->faces[f];

                for(int v = 0; v < arrlen(face_geo->vertices); v++) {
                    brush->center = vec3_add(brush->center, face_geo->vertices[v].pos);
                    vertex_count++;
                }
            }

            if(vertex_count > 0) {
                brush->center = vec3_div_double(brush->center, vertex_count);
            }

            ent->center = vec3_add(ent->center, brush->center);
        }

        if(arrlen(ent->b) > 0) {
            ent->center = vec3_div_double(ent->center, arrlen(ent->b));
        }
    }

    for(int e = 0; e < arrlen(map->entitys); e++) {
        QMapEntity *ent = &(map->entitys[e]);
        QMapEntityGeometry *ent_geo = &(geo.entitys[e]);

        for(int b = 0; b < arrlen(ent->b); b++) {
            QMapBrush *brush = &ent->b[b];
            QMapBrushGeometry *brush_geo = &ent_geo->brushes[b];

            for(int f = 0; f < arrlen(brush->faces); f++) {
                QMapFace *face = &brush->faces[f];
                QMapFaceGeometry *face_geo = &brush_geo->faces[f];

                if(arrlen(face_geo->vertices) < 3) {
                    continue;
                }

                wind_map_data = map;
                wind_map_geo = &geo;

                wind_entity_idx = e;
                wind_brush_idx = b;
                wind_face_idx = f;

                wind_face_basis = vec3_sub(face_geo->vertices[1].pos, face_geo->vertices[0].pos);
                wind_face_center = (DoubleVector3){0};
                wind_face_normal = face->normal;
                
                for(int v = 0; v < arrlen(face_geo->vertices); v++) {
                    wind_face_center = vec3_add(wind_face_center, face_geo->vertices[v].pos);
                }

                wind_face_center = vec3_div_double(wind_face_center, arrlen(face_geo->vertices));

                qsort(face_geo->vertices, arrlen(face_geo->vertices), sizeof(QMapFaceVertex), sort_vertices_by_winding);

                wind_entity_idx = 0;
            }
        }
    }

    for(int e = 0; e < arrlen(map->entitys); e++) {
        QMapEntity *entity = &map->entitys[e];
        QMapEntityGeometry *entity_geo = &geo.entitys[e];

        for(int b = 0; b < arrlen(entity->b); b++) {
            QMapBrush *brush = &entity->b[b];
            QMapBrushGeometry *brush_geo = &entity_geo->brushes[b];

            for(int f = 0; f < arrlen(brush->faces); f++) {
                QMapFaceGeometry *face_geo = &brush_geo->faces[f];

                if(arrlen(face_geo->vertices) < 3)
                    continue;
                
                // int tmp = arrlen(face_geo->vertices);

                for(int i = 0; i < arrlen(face_geo->vertices) - 2; i++) {
                    arrput(face_geo->indices, 0);
                    arrput(face_geo->indices, i + 1);
                    arrput(face_geo->indices, i + 2);
                }
            }
        }
    }

    return geo;
}

void map_generate_brush_geometry(QMapBrush *b, QMapBrushGeometry *g, bool phong, QMapData *m) {
    // printf("There are %d faces in this brush\n", arrlen(b->faces));
    for(int f0 = 0; f0 < arrlen(b->faces); f0++) {
        for(int f1 = 0; f1 < arrlen(b->faces); f1++) {
            for(int f2 = 0; f2 < arrlen(b->faces); f2++) {
                DoubleVector3 vertex = (DoubleVector3) {0, 0, 0};
                
                if(intersect_faces(b->faces[f0],b->faces[f1],b->faces[f2], &vertex)) {
                    if(vertex_in_hull(b->faces, arrlen(b->faces), vertex)) {
                    // if(true) {
                        
                        QMapFace *face = &(b->faces[f0]);
                        QMapFaceGeometry *face_geo = &g->faces[f0];

                        DoubleVector3 normal;

                        if(phong) {
                            // TODO add support for _phong_angle (maybe?)

                            normal = vec3_normalize(
                                vec3_add(
                                    vec3_add(
                                        b->faces[f0].normal,
                                        b->faces[f1].normal
                                    ),
                                    b->faces[f2].normal
                                )
                            );
                        } else {
                            normal = face->normal;
                        }

                        Vector2 uv = map_get_uv(vertex, face, m->textureSizes[face->texture]);
                        Vector4 tangent = map_get_tangent(face);

                        bool unique_vertex = true;
                        int duplicate_index = -1;

                        for(int v = 0; v < arrlen(face_geo->vertices); v++) {
                            DoubleVector3 comp_vertex = face_geo->vertices[v].pos;
                            if(vec3_length(vec3_sub(vertex, comp_vertex)) < CMP_EPSILON) {
                                unique_vertex = false;
                                duplicate_index = v;
                                break;
                            }
                        }

                        if(unique_vertex) {
                            arrput(face_geo->vertices, ((QMapFaceVertex) {vertex, normal, uv, tangent}));
                        } else if(phong){
                            face_geo->vertices[duplicate_index].normal = vec3_add(face_geo->vertices[duplicate_index].normal, normal);
                        }
                    }
                }
            }
        }
    }

    for(int f = 0; f < arrlen(b->faces); f++) {
        QMapFaceGeometry *face_geo = &g->faces[f];

        for(int v = 0; v < arrlen(face_geo->vertices); v++) {
            face_geo->vertices[v].normal = vec3_normalize(face_geo->vertices[v].normal);
        }
    }
}


bool intersect_faces(QMapFace f0, QMapFace f1, QMapFace f2, DoubleVector3 *o_vertex) {
    DoubleVector3 normal0 = f0.normal;
    DoubleVector3 normal1 = f1.normal;
    DoubleVector3 normal2 = f2.normal;

    // DoubleVector3 tmp = vec3_cross(normal0, normal1);

    double denom = vec3_dot(vec3_cross(normal0, normal1), normal2);

    // if() {
    //     printf("???\n");
    // }

    if (denom < CMP_EPSILON) {
        return false;
    }

    if (o_vertex) {
        *o_vertex = vec3_div_double(
            vec3_add(
                vec3_add(
                    vec3_mul_double(
                        vec3_cross(normal1, normal2),
                        f0.dist),
                    vec3_mul_double(
                        vec3_cross(normal2, normal0),
                        f1.dist)),
                vec3_mul_double(
                    vec3_cross(normal0, normal1),
                    f2.dist)),
            denom);
    }

    return true;
}

bool vertex_in_hull(QMapFace *faces, int face_count, DoubleVector3 vertex) {
    for (int f = 0; f < face_count; f++) {
        QMapFace face_inst = faces[f];
        double proj = vec3_dot(face_inst.normal, vertex);

        if (proj > face_inst.dist && fabs(face_inst.dist - proj) > CMP_EPSILON) {
            return false;
        }
    }

    return true;
}

Vector2 map_get_uv(DoubleVector3 vertex, const QMapFace *f, Vector2 texSize) {
    Vector2 uv;
    if(f->isValve) {

        DoubleVector3 u_axis = f->vU.axis;
        DoubleVector3 v_axis = f->vV.axis;

        double u_shift = f->vU.offset;
        double v_shift = f->vV.offset;

        uv.x = vec3_dot(u_axis, vertex);
        uv.y = vec3_dot(v_axis, vertex);

        uv.x /= texSize.x;
        uv.y /= texSize.y;

        uv.x /= f->tScaleX;
        uv.y /= f->tScaleY;

        uv.x += u_shift / texSize.x;
        uv.y += v_shift / texSize.y;

    } else {
        double du = fabs(vec3_dot(f->normal, (DoubleVector3) {0, 0, 1}));
        double dr = fabs(vec3_dot(f->normal, (DoubleVector3) {0, 1, 0}));
        double df = fabs(vec3_dot(f->normal, (DoubleVector3) {1, 0, 0}));

        if(du >= dr && du >= df)
            uv = (Vector2) {(float) vertex.x, (float) -vertex.y};
        else if(dr >= du && dr >= df)
            uv = (Vector2) {(float) vertex.x, (float) -vertex.z};
        else if(df >= du && df >= dr)
            uv = (Vector2) {(float) vertex.y, (float) -vertex.z};

        Vector2 rotated;
        double angle = DEG_TO_RAD(f->tRot);
        rotated.x = uv.x * cos(angle) - uv.y * sin(angle);
        rotated.y = uv.x * sin(angle) + uv.y * cos(angle);
        uv = rotated;

        uv.x /= texSize.x;
        uv.y /= texSize.y;

        uv.x /= f->tScaleX;
        uv.y /= f->tScaleY;

        uv.x += f->sU / texSize.x;
        uv.y += f->sV / texSize.y;
    }

    return uv;
}

double sign(double v)
{
    if (v > 0)
    {
        return 1.0;
    }
    else if (v < 0)
    {
        return -1.0;
    }

    return 0.0;
}

Vector4 map_get_tangent(QMapFace *face) {
    Vector4 t;

    if(face->isValve) {
        DoubleVector3 u_axis = vec3_normalize(face->vU.axis);
        DoubleVector3 v_axis = vec3_normalize(face->vV.axis);

        double v_sign = -sign(vec3_dot(vec3_cross(face->normal, u_axis), v_axis));

        t.x = u_axis.x;
        t.y = u_axis.y;
        t.z = u_axis.z;
        t.w = v_sign;
    } else {
        double du = vec3_dot(face->normal, (DoubleVector3) {(0.0), (0.0), (1.0)});
        double dr = vec3_dot(face->normal, (DoubleVector3) {(0.0), (1.0), (0.0)});
        double df = vec3_dot(face->normal, (DoubleVector3) {(1.0), (0.0), (0.0)});

        double dua = fabs(du);
        double dra = fabs(dr);
        double dfa = fabs(df);

        DoubleVector3 u_axis;
        double v_sign = 0;

        if(dua >= dra && dua >= dfa) {
            u_axis = (DoubleVector3) {0, 0, 1};
            v_sign = sign(du);
        } else if(dra >= dua && dra >= dfa) {
            u_axis = (DoubleVector3) {0, 0, 1};
            v_sign = -sign(dr);
        } else if(dfa >= dua && dfa >= dra) {
            u_axis = (DoubleVector3) {0, 1, 0};
            v_sign = sign(df);
        }

        v_sign *= sign(face->tScaleY);
        u_axis = vec3_rotate(u_axis, face->normal, -face->tRot * v_sign);

        t.x = u_axis.x;
        t.y = u_axis.y;
        t.z = u_axis.z;
        t.w = v_sign;
    }

    return t;
}

void map_print_geometry(const QMapData *map, const QMapMapGeometry *geo) {
    printf("Printing Geometry Data\n--------------------\n");

    for(int e = 0; e < arrlen(map->entitys); e++) {
        QMapEntity *ent = &map->entitys[e];
        QMapEntityGeometry *ent_geo = &geo->entitys[e];

        printf("[ + QMapEntity ]\n");

        for(int b = 0; b < arrlen(ent->b); b++) {
            QMapBrush *brush = &ent->b[b];
            QMapBrushGeometry *brush_geo = &ent_geo->brushes[b];

            printf("\t[ + QMapBrush ]\n");
            for(int f = 0; f < arrlen(brush->faces); f++) {
                // QMapFace *face = &brush->faces[f];
                QMapFaceGeometry *face_geo = &brush_geo->faces[f];

                printf("\t\t[ ");

                for(int v = 0; v < arrlen(face_geo->vertices); v++) {
                    QMapFaceVertex vert = face_geo->vertices[v];
                    printf("(%f, %f, %f) ", vert.pos.x, vert.pos.y, vert.pos.z);
                }

                printf("]\n");
            }
            printf("\t[ -QMapBrush ]\n");
        }
        printf("[ - QMapEntity ]\n");
    }
}

void map_face_geo_free(QMapFaceGeometry geometry) {
    free(geometry.indices);
    free(geometry.vertices);
}

void map_brush_geo_free(QMapBrushGeometry brush_geo) {
    for(int f = 0; f < arrlen(brush_geo.faces); f++) {
        map_face_geo_free(brush_geo.faces[f]);
    }
}

void map_entity_geo_free(QMapEntityGeometry ent) {
    for(int b = 0; b < arrlen(ent.brushes); b++) {
        map_brush_geo_free(ent.brushes[b]);
    }
}

void map_geometry_free(QMapMapGeometry map_geo) {
    for(int e = 0; e < arrlen(map_geo.entitys); e++) {
        map_entity_geo_free(map_geo.entitys[e]);
    }
}

QMapEntityGeometry map_entity_geo_init() {
    QMapEntityGeometry e;
    e.brushes = NULL;
    return e;
}

QMapBrushGeometry map_brush_geo_init() {
    QMapBrushGeometry b;
    b.faces = NULL;
    return b;
}

QMapFaceGeometry map_face_geo_init() {
    QMapFaceGeometry f;
    f.vertices = NULL;
    f.indices = NULL;
    return f;
}

#endif
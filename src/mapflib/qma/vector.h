#ifndef MAP_VECTOR_H
#define MAP_VECTOR_H

#endif

#ifdef QMAP_IMP

bool vec3_equals(DoubleVector3 lhs, DoubleVector3 rhs) {
    return vec3_length(vec3_sub(lhs, rhs)) < CMP_EPSILON;
}

DoubleVector3 vec3_add(DoubleVector3 lhs, DoubleVector3 rhs) {
    return (DoubleVector3){
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}

DoubleVector3 vec3_sub(DoubleVector3 lhs, DoubleVector3 rhs) {
    return (DoubleVector3){
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    };
}

DoubleVector3 vec3_mul(DoubleVector3 lhs, DoubleVector3 rhs) {
    return (DoubleVector3){
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z
    };
}

DoubleVector3 vec3_div(DoubleVector3 lhs, DoubleVector3 rhs) {
    return (DoubleVector3){
        lhs.x / rhs.x,
        lhs.y / rhs.y,
        lhs.z / rhs.z
    };
}

double vec3_sqlen(DoubleVector3 v) {
    return vec3_dot(v, v);
}

double vec3_length(DoubleVector3 v) {
    return (double)sqrt(vec3_sqlen(v));
}

DoubleVector3 vec3_normalize(DoubleVector3 v) {
    double len = vec3_length(v);
    return vec3_div_double(v, len);
}

double vec3_dot(DoubleVector3 lhs, DoubleVector3 rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

DoubleVector3 vec3_cross(DoubleVector3 lhs, DoubleVector3 rhs) {
    return (DoubleVector3){
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x,
    };
}

DoubleVector3 vec3_rotate(DoubleVector3 v, DoubleVector3 axis, double angle) {
    mat4 mat = rotation_matrix(axis, angle);
    return mat4_mul_vec3(mat, v);
}

DoubleVector3 vec3_add_double(const DoubleVector3 lhs, const double rhs) {
    DoubleVector3 result = {lhs.x + rhs, lhs.y + rhs, lhs.z + rhs};
    return result;
};

DoubleVector3 vec3_sub_double(const DoubleVector3 lhs, const double rhs) {
    DoubleVector3 result = {lhs.x - rhs, lhs.y - rhs, lhs.z - rhs};
    return result;
};

DoubleVector3 vec3_mul_double(const DoubleVector3 lhs, const double rhs) {
    DoubleVector3 result = {lhs.x * rhs, lhs.y * rhs, lhs.z * rhs};
    return result;
};

DoubleVector3 vec3_div_double(const DoubleVector3 lhs, const double rhs) {
    DoubleVector3 result = {lhs.x / rhs, lhs.y / rhs, lhs.z / rhs};
    return result;
};

#endif
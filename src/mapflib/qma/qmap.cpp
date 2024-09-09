/**
 * This file is just here because qmap throws out a lot of warnings when it is loaded,
 * which is really annoying when you are looking for an erorr
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define STB_DS_IMPLEMENTATION
// #define QMAP_IMP

#include "raymath.h"
#include "stb_ds.h"

#define QMAP_IMP
#include "qmap.h"
// #include "surfacequery.h"
#undef QMAP_IMP
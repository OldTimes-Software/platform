/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#pragma once

#include "platform.h"
#include "platform_math.h"
#include "platform_graphics.h"

#define PLMODEL_MAX_MESHES  32
#define PLMODEL_MAX_FRAMES  512

// Standard mesh
typedef struct PLModel {
    unsigned int num_triangles;
    unsigned int num_vertices;
    unsigned int num_meshes;
    unsigned int num_animations;

    PLMesh *meshes[PLMODEL_MAX_MESHES];

    PLAABB bounds;
} PLModel;

// Per-vertex animated mesh.
typedef struct PLModelFrame {
    unsigned int num_meshes;

    PLMesh *meshes[PLMODEL_MAX_MESHES];

    PLAABB bounds;
} PLModelFrame;

typedef struct PLAnimatedModel {
    unsigned int num_triangles;
    unsigned int num_vertices;
    unsigned int num_frames;

    PLMeshPrimitive primitive;

    PLModelFrame frames[PLMODEL_MAX_FRAMES];
} PLAnimatedModel;

PL_EXTERN_C

PLModel *plLoadModel(const char *path);

void plDeleteModel(PLModel *model);
void plDrawModel(PLModel *model);

PL_EXTERN void plRegisterModelLoader(const char *ext, PLModel*(*LoadFunction)(const char *path));

PL_EXTERN void plGenerateModelNormals(PLModel *model);
PL_EXTERN void plGenerateModelAABB(PLModel *model);
PL_EXTERN void plGenerateAnimatedModelNormals(PLAnimatedModel *model);

PL_EXTERN_C_END

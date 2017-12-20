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
#include "model_private.h"

#include <PL/platform_filesystem.h>

/*	PLATFORM MODEL LOADER	*/

typedef struct ModelInterface {
    const char *ext;
    PLModel*(*LoadFunction)(const char *path);
} ModelInterface;

ModelInterface model_interfaces[512]= {
        { "mdl", LoadRequiemModel },
        //{ "vtx", _plLoadStaticHOWModel },
        //{ "mdl", _plLoadStaticSourceModel },
        //{ "mdl", _plLoadStaticGoldSrcModel },
        //{ "smd", _plLoadStaticSMDModel },
        //{ "obj", _plLoadOBJModel },
};
unsigned int num_model_interfaces = 1;

///////////////////////////////////////

void plGenerateModelNormals(PLModel *model) {
    plAssert(model);
    for(unsigned int i = 0; i < model->num_meshes; ++i) {
        plGenerateMeshNormals(model->meshes[i]);
    }
}

void plGenerateModelAABB(PLModel *model) {
    plAssert(model);
    for(unsigned int i = 0; i < model->num_meshes; ++i) {
        plAddAABB(&model->bounds, plCalculateMeshAABB(model->meshes[i]));
    }
}

void plGenerateAnimatedModelNormals(PLAnimatedModel *model) {
    plAssert(model);
    for (PLModelFrame *frame = &model->frames[0]; frame; ++frame) {
        for(unsigned int i = 0; i < frame->num_meshes; ++i) {
            plGenerateMeshNormals(frame->meshes[i]);
        }
    }
}

///////////////////////////////////////

void plRegisterModelLoader(const char *ext, PLModel*(*LoadFunction)(const char *path)) {
    model_interfaces[num_model_interfaces].ext = ext;
    model_interfaces[num_model_interfaces].LoadFunction = LoadFunction;
    num_model_interfaces++;
}

PLModel *plLoadModel(const char *path) {
    if(!plFileExists(path)) {
        ReportError(PL_RESULT_FILEREAD, "Failed to load model, %s!", path);
        return NULL;
    }

    const char *extension = plGetFileExtension(path);
    for(unsigned int i = 0; i < num_model_interfaces; ++i) {
        if(model_interfaces[i].LoadFunction == NULL) {
            continue;
        }

        if(model_interfaces[i].ext[0] != '\0') {
            if (!strcmp(extension, model_interfaces[i].ext)) {
                PLModel *model = model_interfaces[i].LoadFunction(path);
                if(model != NULL) {
                    return model;
                }
            }
        }
    }

    return NULL;
}

void plDeleteModel(PLModel *model) {
    plAssert(model);
    for(unsigned int i = 0; i < model->num_meshes; ++i) {
        if(model->meshes[i] == NULL) {
            continue;
        }

        plDeleteMesh(model->meshes[i]);
    }

    free(model);
}

void plDrawModel(PLModel *model) {
    plAssert(model);
    for(unsigned int i = 0; i < model->num_meshes; ++i) {
        plDrawMesh(model->meshes[i]);
    }
}

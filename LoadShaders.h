#pragma once

#include "stdafx.h"

struct ShaderInfo {
    GLenum type;
    const char * filename;
    GLuint shader;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

GLuint LoadShaders(ShaderInfo* shaders);

#ifdef __cplusplus
}
#endif // __cplusplus
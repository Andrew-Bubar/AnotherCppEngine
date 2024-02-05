#pragma once 

#include "glcorearb.h"
#include "AnotherCppEngine_lib.h"

static PFNGLCREATEPROGRAMPROC glCreateProgram_ptr;

void gl_load_functions(){


    glCreateProgram_ptr = 
    (PFNGLCREATEPROGRAMPROC)platform_load_gl_function("glCreateProgram");
}

GLAPI GLuint APIENTRY glCreateProgram(void){

    return glCreateProgram_ptr();
}

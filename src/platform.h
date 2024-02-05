
#pragma once

/*
********************************************************************************
            All the varribles for more general systems and varribles
********************************************************************************
*/

static bool isRunning; //for the while loop

// Platform Functions
bool platform_create_windows(int width, int height, char* title);
void platform_update_window();
void* platform_load_gl_function(char* funcName);
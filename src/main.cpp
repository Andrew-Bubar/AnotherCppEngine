
#include "AnotherCppEngine_lib.h"
#include "platform.h"

//Windows Platform Functions
#ifdef _WIN32 //is the OS windows?
#include "win32_platform.cpp"
#endif

#include "gl_renderer.h" //this has to be after the platform functions so it loads properly


int main(){

    if(!platform_create_windows(1200, 720, "AnotherCppEngine")){
        SM_ASSERT(false, "Not opened window");
    }

    while(isRunning){
        platform_update_window();
    }

    return 0;
}

#include "AnotherCppEngine_lib.h"
#include "platform.h"

//Windows Platform Functions
#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.h"


int main(){

    if(!platform_create_windows(1200, 720, "AnotherCppEngine")){
        SM_ASSERT(false, "Not opened window");
    }

    while(isRunning){
        platform_update_window();
    }

    return 0;
}
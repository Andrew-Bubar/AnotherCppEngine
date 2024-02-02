
#include "AnotherCppEngine_lib.h"

// Platform Functions
bool platform_create_windows(int width, int height, char* title);
void platform_update_window();

//Windows Platform Functions
#ifdef _WIN32
#define WIN32_LEANE_AND_MEAN
#define NOMINMAX
#include <Windows.h>


//Windows Globals
static HWND window;
static bool isRunning = true;

// Platform Implementations
LRESULT CALLBACK windows_window_callback(HWND window, UINT msg, 
                                         WPARAM wParam, LPARAM lParam
){
    LRESULT result = 0;

    //handle input
    switch(msg){
        case WM_CLOSE:
        {
            isRunning = false;
            break;
        }

        default:
        {
            result = DefWindowProcA(window, msg, wParam, lParam);
        }
    }
    return result;
}

bool platform_create_windows(int width, int height, char* title){

    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASSA wc = {};
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = title;
    wc.lpfnWndProc = windows_window_callback;

    if(!RegisterClassA(&wc)){
        return false;
    }

    int dwStyle = WS_OVERLAPPEDWINDOW;

    window = CreateWindowExA(0,
    title,      //this is the lpsz from the wc class
    title,      //this is the title bar string
    dwStyle,    //this is how the window looks
    100,
    100,
    width,      //Size of Window
    height,
    NULL,       //parent?
    NULL,       //Menu
    instance,
    NULL
    );

    if(window == NULL){
        return false;
    }

    ShowWindow(window, SW_SHOW);

    return true;
}

void platform_update_window(){

    MSG msg;

    while(PeekMessageA(&msg, window, 0, 0, PM_REMOVE)){
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

#endif



int main(){

    if(!platform_create_windows(1200, 720, "AnotherCppEngine")){
        
    }

    SM_TRACE("test");
    SM_WARN("test");
    SM_ERROR("Test");

    while(isRunning){
        platform_update_window();
        SM_ASSERT(false, "Assert not hit!");
    }

    return 0;
}
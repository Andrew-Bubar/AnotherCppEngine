
#define WIN32_LEANE_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include "platform.h"
#include "AnotherCppEngine_lib.h"


//Windows Globals
static HWND window;

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

bool platform_create_windows(int width, int height, char* title){ //making the window for ... Windows

    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASSA wc = {}; //making an empty window class

    //setting up the class
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = title;
    wc.lpfnWndProc = windows_window_callback;

    if(!RegisterClassA(&wc)){ //did window class work?
        return false;
    }

    int dwStyle = WS_OVERLAPPEDWINDOW;

    //Making the window
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

    ShowWindow(window, SW_SHOW); //show the window

    return true;
}

void platform_update_window(){ //take in inputs for the window

    MSG msg;

    while(PeekMessageA(&msg, window, 0, 0, PM_REMOVE)){
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

void* platform_load_gl_function(char* funcName){ //a general function for loading opengl things
    PROC proc = wglGetProcAddress(funcName); //get the class as char array

    if(!proc){ //if it didnt load

        static HMODULE openglDLL = LoadLibraryA("opengl32.dll"); //get the raw dll from the GPU driver

        proc =  GetProcAddress(openglDLL, funcName); //try again

        if(!proc){ //if didn't
            SM_ASSERT(false, "flailed to load open gl %s", "glCreateProgram");
            return nullptr; //return null
        }
    }

    return (void*)proc; //output the function
}

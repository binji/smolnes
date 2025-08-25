#include "MiniFB.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../thirdparty/wacup/wa_ipc.h" // wacup ipc api

#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static WNDCLASS s_wc;
static HWND s_wnd;
static int s_close = 0;
static int s_width;
static int s_height;
static int s_scale = 1;
static HDC s_hdc;
static void* s_buffer;
static BITMAPINFO* s_bitmapInfo;
static char key_status[512] = {};

extern In_Module mod;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT res = 0;

    switch (message) {
        case WM_PAINT: {
            if (s_buffer) {
                StretchDIBits(s_hdc, 0, 0, s_width * s_scale, s_height * s_scale, 0, 0, s_width, s_height, s_buffer,
                              s_bitmapInfo, DIB_RGB_COLORS, SRCCOPY);

                ValidateRect(hWnd, NULL);
            }

            break;
        }

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            key_status[wParam] = !(lParam >> 31 & 1);

            if ((wParam & 0xFF) == 27)
                s_close = 1;

            break;
        }

        case WM_CLOSE: {
            s_close = 1;
            break;
        }

        default: {
            res = DefWindowProc(hWnd, message, wParam, lParam);
        }
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

embedWindowState myWindowState;
HWND hMainWnd = NULL;

const char* title;

int mfb_open(const char* title, int width, int height, int scale) {
    int styles;
    HWND parent = NULL;
    HWND (*e)(embedWindowState *v);

    title = title;

    myWindowState.flags = EMBED_FLAGS_NOWINDOWMENU | EMBED_FLAGS_SCALEABLE_WND;
    myWindowState.r.left = 0;
    myWindowState.r.top = 0;
    myWindowState.r.right = width * scale;
    myWindowState.r.bottom = height * scale;

    myWindowState.r.right -= myWindowState.r.left;
    myWindowState.r.bottom -= myWindowState.r.top;

    *(void**)&e = (void *)SendMessage(mod.hMainWindow,WM_WA_IPC,(LPARAM)0,IPC_GET_EMBEDIF);

    if (!e)
    {
		MessageBox(mod.hMainWindow,"This plugin requires Winamp 5.0+","blah",MB_OK);
		return 1;
    }

    parent = e(&myWindowState);

    SetWindowText(myWindowState.me, title); // set our window title

    {	// Register our window class
		WNDCLASS wc;
		memset(&wc,0,sizeof(wc));
		wc.lpfnWndProc = WndProc;				// our window procedure
		wc.hInstance = mod.hDllInstance;	// hInstance of DLL
		wc.lpszClassName = title;			// our window class name
	
		if (!RegisterClass(&wc)) 
		{
			MessageBox(mod.hMainWindow,"Error registering window class","blah",MB_OK);
			return 1;
		}
	}

    s_width = width;
    s_height = height;
    s_scale = scale;

    styles = WS_VISIBLE|WS_CHILDWINDOW|WS_OVERLAPPED|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
        // Create the window
        s_wnd = CreateWindowEx(
            0,	// these exstyles put a nice small frame, 
            // but also a button in the taskbar
            title,		    // our window class name
            NULL,				// no title, we're a child
            styles,	            // styles, we are a child	
            0, 0, myWindowState.r.right - myWindowState.r.left, myWindowState.r.bottom - myWindowState.r.top, // Position and size
            parent,             // Parent window
            NULL,               // Menu
            mod.hDllInstance, // Instance handle
            0 // Additional application data
        );
#ifdef _WIN64
    // Store pointer safely on 64-bit
    SetWindowLongPtr(s_wnd, GWLP_USERDATA, (LONG_PTR)&mod);
#else
    // Store pointer safely on 32-bit
    SetWindowLong(s_wnd, GWL_USERDATA, (LONG)(INT_PTR)&mod);
#endif
        //SendMessage(this_mod->hwndParent, WM_WA_IPC, (int)hMainWnd, IPC_SETVISWND);

        if (s_wnd) {
            ShowWindow(parent,SW_SHOWNORMAL);
        }

    s_bitmapInfo = (BITMAPINFO *)calloc(1, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 3);
    s_bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    s_bitmapInfo->bmiHeader.biPlanes = 1;
    s_bitmapInfo->bmiHeader.biBitCount = 16;
    s_bitmapInfo->bmiHeader.biCompression = BI_BITFIELDS;
    s_bitmapInfo->bmiHeader.biWidth = width;
    s_bitmapInfo->bmiHeader.biHeight = -height;

    ((DWORD *)s_bitmapInfo->bmiColors)[2] = 0xF800;
    ((DWORD *)s_bitmapInfo->bmiColors)[1] = 0x07E0;
    ((DWORD *)s_bitmapInfo->bmiColors)[0] = 0x001F;
    s_hdc = GetDC(s_wnd);

    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int mfb_update(void* buffer, int fps_limit) {
    static DWORD previousFrameTime = 0;
    MSG msg;

    s_buffer = buffer;

    InvalidateRect(s_wnd, NULL, TRUE);
    SendMessage(s_wnd, WM_PAINT, 0, 0);

    while (PeekMessage(&msg, s_wnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (s_close == 1)
        return -1;

    if (fps_limit) {
        const DWORD targetFrameTime = 1000 / fps_limit;
        const DWORD elapsedFrameTime = GetTickCount() - previousFrameTime;

        if (elapsedFrameTime < targetFrameTime) {
            Sleep(targetFrameTime - elapsedFrameTime);
        }

        previousFrameTime = GetTickCount();
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mfb_close() {
    s_buffer = 0;
    free(s_bitmapInfo);
    ReleaseDC(s_wnd, s_hdc);
    if (myWindowState.me) 
    {
        SetForegroundWindow(mod.hMainWindow);
        DestroyWindow(myWindowState.me);
    }
    UnregisterClass(title,mod.hDllInstance); // unregister window class
}

char * mfb_keystatus() {
    return key_status;
}

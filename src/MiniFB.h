#ifndef _MINIFB_H_
#define _MINIFB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "../thirdparty/wacup/in2.h" // wacup input plugin api
#include "../thirdparty/wacup/wa_ipc.h" // wacup ipc api

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MFB_RGB(r, g, b) (((unsigned int)r) << 16) | (((unsigned int)g) << 8) | b

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Create a window that is used to display the buffer sent into the mfb_update function, returns 0 if fails
int mfb_open(const char* name, int width, int height, int scale);

// Update the display. Input buffer is assumed to be a 32-bit buffer of the size given in the open call
// Will return -1 when ESC key is pressed (later on will return keycode and -1 on other close signal) 
int mfb_update(void* buffer, int fps_limit);

// Close the window
void mfb_close();
char * mfb_keystatus();
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif

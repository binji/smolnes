#ifndef NULLSOFT_WINAMP_IN2H
#define NULLSOFT_WINAMP_IN2H
// Input plugin interface

#include "out.h"
#ifdef __cplusplus
class api_service;
#endif

// If you want your input plugin to support unicode then define the following which will then
// adjust required functions to their unicode variants. This is supported from Winamp 5.3+.
#define IN_UNICODE   0x0F000000
#define IN_INIT_RET  0xF0000000

#ifdef UNICODE_INPUT_PLUGIN
#define in_char wchar_t
#define IN_VER_OLD (IN_UNICODE | 0x100)
#define IN_VER (IN_UNICODE | 0x101)
#define IN_VER_RET (IN_INIT_RET | IN_VER)
#else
#define in_char char
#define IN_VER_OLD 0x100
#define IN_VER 0x101
#define IN_VER_RET (IN_INIT_RET | IN_VER)
#endif

// added 5.64+ & updated 5.66+
//
// specify IN_VER if you want to provide a unicode (wchar_t*) description and only work on 5.64+
// specify IN_VER_OLD to use the original (char*) description as before
// note: we are using the fact that sizeof(char*) == sizeof(wchar_t*) to be able to allow this
//       so now when using IN_VER you will need to cast description to (wchar_t*) to set this.

// added 5.66+
// specify IN_VER_RET to allow for the init(..) member to return an int status value as either
// IN_INIT_SUCCESS or IN_INIT_FAILURE to allow for better support with loading failures or if
// needing to specifically prevent loading if required e.g. OS incompatibility or access issues.
//
// Also added is the 'service' member which saves sending a IPC_GET_API_SERVICE call on loading
// which will be filled in if IN_VER_RET is specified in the 'version' member of the plug-in.

#define IN_MODULE_FLAG_USES_OUTPUT_PLUGIN 1

// By default Winamp assumes your input plugin wants to use Winamp's EQ, and doesn't do replay gain
// if you handle any of these yourself (EQ, Replay Gain adjustments), then set these flags accordingly

// Set this if you want to implement your own EQ inplace of using Winamp's native implementation.
#define IN_MODULE_FLAG_EQ 2

// Set this if you adjusted volume for replay gain. For tracks with no replay gain metadata then you
// should clear this flag UNLESS you handle "non_replaygain" gain adjustment yourself then keep it.
#define IN_MODULE_FLAG_REPLAYGAIN 8 

// Use this if you queried for the replay gain preamp parameter and used it. This is new to 5.54 clients.
#define IN_MODULE_FLAG_REPLAYGAIN_PREAMP 16
typedef struct 
{
  int version;              // module type (IN_VER)
  char *description;        // description of module, with version string

  HWND hMainWindow;         // Winamp's main window (filled in by Winamp - is a valid HWND on 5.1+ clients)
  HINSTANCE hDllInstance;   // DLL instance handle (Also filled in by Winamp)

  char *FileExtensions;     // "mp3\0Layer 3 MPEG\0mp2\0Layer 2 MPEG\0mpg\0Layer 1 MPEG\0"
                            // May be altered from Config, so the user can select what they want
	
  int is_seekable;          // is this stream seekable? 
  int UsesOutputPlug;       // does this plug-in use the output plug-ins? (musn't ever change, ever :)
                            // note that this has turned into a "flags" field see IN_MODULE_FLAG_*

  void (__cdecl *Config)(HWND hwndParent); // configuration dialog
  void (__cdecl *About)(HWND hwndParent);  // about dialog

  // 5.66 - changed from void (__cdecl *Init)(); if using IN_VER_RET or IN_VER_RET_OLD
  int (__cdecl *Init)();				// called at program init
  void (__cdecl *Quit)();				// called at program quit

  #define GETFILEINFO_TITLE_LENGTH 2048 
  // If file == NULL then the currently playing file is used (assumes you've cached it as required)
  void (*GetFileInfo)(const in_char *file, in_char *title, int *length_in_ms);

  #define INFOBOX_EDITED 0
  #define INFOBOX_UNCHANGED 1
  int (__cdecl *InfoBox)(const in_char *file, HWND hwndParent);

  int (__cdecl *IsOurFile)(const in_char *fn);    // called before extension checks, to allow detection of mms://, etc

  // playback stuff
  int (__cdecl *Play)(const in_char *fn);  // return zero on success, -1 on file-not-found, some other value on other (stopping Winamp) error
  void (__cdecl *Pause)();                 // pause stream
  void (__cdecl *UnPause)();               // unpause stream
  int (__cdecl *IsPaused)();               // ispaused? return 1 if paused, 0 if not
  void (__cdecl *Stop)();                  // stop (unload) stream

  // time stuff
  int (__cdecl *GetLength)();              // get length in ms
  int (__cdecl *GetOutputTime)();          // returns current output time in ms. (usually returns outMod->GetOutputTime()
  void (__cdecl *SetOutputTime)(int time_in_ms);  // seeks to point in stream (in ms). Usually you signal your thread to seek, which seeks and calls outMod->Flush()..

  // volume stuff
  void (__cdecl *SetVolume)(int volume);   // from 0 to 255.. usually just call outMod->SetVolume
  void (__cdecl *SetPan)(int pan);         // from -127 to 127.. usually just call outMod->SetPan
	
  // in-window builtin vis stuff
  void (__cdecl *SAVSAInit)(int maxlatency_in_ms, int srate);  // call once in Play(). maxlatency_in_ms should be the value returned from outMod->Open()
  // call after opening audio device with max latency in ms and samplerate
  void (__cdecl *SAVSADeInit)();           // call in Stop()

  // simple vis supplying mode
  void (__cdecl *SAAddPCMData)(void *PCMData, int nch, int bps, int timestamp);    // sets the spec data directly from PCM data quick and easy way
                                   // to get vis working :)  needs at least 576 samples :)

  // advanced vis supplying mode, only use if you're cool. Use SAAddPCMData for most stuff.
  int (__cdecl *SAGetMode)();       // gets csa (the current type (4=ws,2=osc,1=spec))  use when calling SAAdd()
  int (__cdecl *SAAdd)(void *data, int timestamp, int csa);    // sets the spec data, filled in by Winamp

  // vis stuff (plug-in)
  // simple vis supplying mode
  void (__cdecl *VSAAddPCMData)(void *PCMData, int nch, int bps, int timestamp);   // sets the vis data directly from PCM data  quick and easy way
                                   // to get vis working :)  needs at least 576 samples :)

  // advanced vis supplying mode, only use if you're cool. Use VSAAddPCMData for most stuff.
  int (__cdecl *VSAGetMode)(int *specNch, int *waveNch);       // use to figure out what to give to VSAAdd
  int (__cdecl *VSAAdd)(void *data, int timestamp);            // filled in by Winamp, called by plug-in

  // call this in Play() to tell the vis plug-ins the current output params. 
  void (__cdecl *VSASetInfo)(int srate, int nch); // <-- Correct (benski, dec 2005).. old declaration had the params backwards

  // dsp plug-in processing: 
  // (filled in by Winamp, calld by input plug)

  // returns 1 if active (which means that the number of samples returned by dsp_dosamples could be
  // greater than went in.. Use it to estimate if you'll have enough room in the output buffer
  int (__cdecl *dsp_isactive)();

  // returns number of samples to output. This can be as much as twice numsamples. 
  // be sure to allocate enough buffer for samples, then.
  int (__cdecl *dsp_dosamples)(short int *samples, int numsamples, int bps, int nch, int srate);

  // eq stuff
  void (__cdecl *EQSet)(int on, char data[10], int preamp); // 0-64 each, 31 is +0, 0 is +12, 63 is -12. Do nothing to ignore.

  // info setting (filled in by Winamp)
  void (__cdecl *SetInfo)(int bitrate, int srate, int stereo, int synched); // if -1, changes ignored? :)

  Out_Module *outMod; // filled in by Winamp, optionally used :)

  // filled in by Winamp (added 5.66+ to replace need to call IPC_GET_API_SERVICE on loading)
  #ifdef __cplusplus
  api_service *service;
  #else
  void * service;
  #endif
} In_Module;

// added 5.66+
// return values from the init(..) which determines if Winamp will continue loading
// and handling the plugin or if it will disregard the load attempt. If GEN_INIT_FAILURE
// is returned then the plugin will be listed as [NOT LOADED] on the plug-in prefs page.
#define IN_INIT_SUCCESS 0
#define IN_INIT_FAILURE 1


// These are the return values to be used with the uninstall plugin export function:
// __declspec(dllexport) int __cdecl winampUninstallPlugin(HINSTANCE hDllInst, HWND hwndDlg, int param)
// which determines if Winamp can uninstall the plugin immediately or on Winamp restart.
// If this is not exported then Winamp will assume an uninstall with reboot is the only way.
//
#define IN_PLUGIN_UNINSTALL_NOW    0x1
#define IN_PLUGIN_UNINSTALL_REBOOT 0x0
//
// Uninstall support was added from 5.0+ and uninstall now support from 5.5+ though note
// that it is down to you to ensure that if uninstall now is returned that it will not
// cause a crash i.e. don't use if you've been subclassing the main window.
//
// The HWND passed in the calling of winampUninstallPlugin(..) is the preference page HWND.
//

// For a input plugin to be correctly detected by Winamp you need to ensure that
// the exported winampGetInModule2(..) is exported as an undecorated function
// e.g.
// #ifdef __cplusplus
//   extern "C" {
// #endif
// __declspec(dllexport) In_Module * __cdecl winampGetInModule2(){ return &plugin; }
// #ifdef __cplusplus
//   }
// #endif
//

#endif
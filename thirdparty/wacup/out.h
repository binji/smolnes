#ifndef NULLSOFT_OUTH
#define NULLSOFT_OUTH
// Output plugin interface

#include <windows.h>
#include <stddef.h>

#if (_MSC_VER <= 1200)
typedef int intptr_t;
#endif

#define OUT_VER 0x10

// added 5.64+
#define OUT_VER_U 0x11
// specify OUT_VER_U if you want to provide a unicode (wchar_t*) description and only work on 5.64+
// specify OUT_VER to use the original (char*) description as before
// note: we are using the fact that sizeof(char*) == sizeof(wchar_t*) to be able to allow this
//       so when using OUT_VER_U you will need to cast description to (wchar_t*) to set

typedef struct 
{
	int version;		// module version (OUT_VER)
	char *description;	// description of module, with version string
	intptr_t id;		// module id. each input module gets its own. non-nullsoft modules should be >= 65536
						// known module ids are (though these may now be deprecated...):
                        // waveout: 32
                        // gapless: 64
                        // xfade: 63
                        // disk: 33
                        // dsound: 38
                        // NULL: 65
                        // mm2: 69

	HWND hMainWindow;			// winamp's main window (filled in by winamp)
	HINSTANCE hDllInstance;		// DLL instance handle (filled in by winamp)

	void (__cdecl *Config)(HWND hwndParent);	// configuration dialog 
	void (__cdecl *About)(HWND hwndParent);		// about dialog

	void (__cdecl *Init)();		// called when loaded
	void (__cdecl *Quit)();		// called when unloaded

	int (__cdecl *Open)(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms); 
	// this returns >=0 on success, <0 on failure

	// NOTE: bufferlenms and prebufferms are ignored in most if not all output plug-ins.
	// ... so don't expect the max latency returned to be what you asked for.
	// returns max latency in ms (0 for diskwriters, etc)
	// bufferlenms and prebufferms must be in ms. 0 to use defaults. 
	// prebufferms must be <= bufferlenms
	// pass bufferlenms==-666 to tell the output plugin that it's clock is going to be used to sync video
	// out_ds turns off silence-eating when -666 is passed

	void (__cdecl *Close)();	// close the ol' output device.

	int (__cdecl *Write)(char *buf, int len);
	// 0 on success. Len == bytes to write (<= 8192 always). buf is straight audio data. 
	// 1 returns not able to write (yet). Non-blocking, always.

	int (__cdecl *CanWrite)();	// returns number of bytes possible to write at a given time. 
								// Never will decrease unless you call Write (or Close, heh)

	int (__cdecl *IsPlaying)();		// non0 if output is still going or if data in buffers waiting to be
									// written (i.e. closing while IsPlaying() returns 1 would truncate the song

	int (__cdecl *Pause)(int pause);	// returns previous pause state

	void (__cdecl *SetVolume)(int volume);		// volume is 0-255
	void (__cdecl *SetPan)(int pan);			// pan is -128 to 128

	void (__cdecl *Flush)(int t);		// flushes buffers and restarts output at time t (in ms) (used for seeking)

	int (__cdecl *GetOutputTime)();		// returns played time in MS
	int (__cdecl *GetWrittenTime)();	// returns time written in MS (used for synching up vis stuff)

} Out_Module;

// These are the return values to be used with the uninstall plugin export function:
// __declspec(dllexport) int __cdecl winampUninstallPlugin(HINSTANCE hDllInst, HWND hwndDlg, int param)
// which determines if Winamp can uninstall the plugin immediately or on winamp restart.
// If this is not exported then Winamp will assume an uninstall with reboot is the only way.
//
// Note: output plugins cannot be uninstalled without a reboot (unlike other plugin types).
//       From Winamp 5.6 and higher, if you do return a non-zero value then it will still be
//       able to uninstall the plug-in (prior to this, returning non-zero prevented uninstall).
//
#define OUT_PLUGIN_UNINSTALL_REBOOT 0x0
//
// Uninstall support was added from 5.0+ and uninstall now support from 5.5+ though note
// that it is down to you to ensure that if uninstall now is returned that it will not
// cause a crash i.e. don't use if you've been subclassing the main window.
//
// The HWND passed in the calling of winampUninstallPlugin(..) is the preference page HWND.
//

// For a output plugin to be correctly detected by Winamp you need to ensure that
// the exported winampGetOutModule(..) is exported as an undecorated function
// e.g.
// #ifdef __cplusplus
//   extern "C" {
// #endif
// __declspec(dllexport) Out_Module * __cdecl winampGetOutModule(){ return &plugin; }
// #ifdef __cplusplus
//   }
// #endif
//


// added 5.64+
// this is used to notify a compatible output plug-in when it is being used or not
// as the currently active output plug-in allowing it to clean up things as needed

// #ifdef __cplusplus
//   extern "C" {
// #endif
// __declspec(dllexport) void __cdecl winampGetOutModeChange(int mode){ }
// #ifdef __cplusplus
//   }
// #endif

// the following are passed to the exported function indicating the state and also
// where the change has been triggered by:

#define OUT_UNSET		0x0		// no longer the active output
#define OUT_SET			0x1		// is becoming the active output
#define OUT_PLAYBACK	0x100	// used when playback begins and an output plug-in
								// is set for an input plug-in or not based on the
								// input plug-in settings when the playback begins

// note: it is possible to get seemingly 'repeat' mode changes as changing of the
//		 output plug-in via the preferences often is not fully applied until the
//		 (re-)starting of playback and the related updating of the input plug-in.

#endif
